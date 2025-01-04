/****************************************** 
-Name: Sneha Karki
-Fall 2024, CS2713 <003>
-dwx106
-Project 2 Part 3: Sauron-sama's Dungeon - Explore the Dungeon
******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define GRID_SIZE 5
#define ROOM_CHAR '*'
#define MONSTER_CHAR 'M'
#define WEAPON_CHAR 'W'
#define PLAYER_CHAR '@'

// Structure to define a Monster with attributes name, health, and damage
typedef struct {
    char name[20];
    int health;
    int damage;
} Monster;

// Structure to define a Weapon with attributes name, damage, and durability
typedef struct {
    char name[20];
    int damage;
    int durability;
} Weapon;

// Structure to define the Dungeon containing a grid, arrays of monsters and weapons, and their counts
typedef struct {
    char grid[GRID_SIZE][GRID_SIZE];
    Monster *monsters;
    int monster_count;
    Weapon *weapons;
    int weapon_count;
} Dungeon;

// Structure to define the Player with health, position coordinates, and a count of monsters defeated
typedef struct {
    int health;
    int pos_x, pos_y;
    int monsters_defeated;
} Player;

// Function to load monster data from a file and initialize monsters in the dungeon
void loadMonstersFromFile(Dungeon *dungeon, int monsterCount) {
    FILE *file = fopen("monsters.txt", "r");
    if (!file) {
        printf("Error opening monsters.txt\n");
        exit(1);
    }
    dungeon->monsters = malloc(monsterCount * sizeof(Monster));
    char line[20];
    for (int i = 0; i < monsterCount && fgets(line, sizeof(line), file); i++) {
        strtok(line, "\n"); // Remove newline character
        strcpy(dungeon->monsters[i].name, line);
        dungeon->monsters[i].health = rand() % 30 + 20; // Random health between 20 and 49
        dungeon->monsters[i].damage = rand() % 10 + 5;  // Random damage between 5 and 14
    }
    fclose(file);
}

// Function to load weapon data from a file and initialize weapons in the dungeon
void loadWeaponsFromFile(Dungeon *dungeon, int weaponCount) {
    FILE *file = fopen("weapons.txt", "r");
    if (!file) {
        printf("Error opening weapons.txt\n");
        exit(1);
    }
    dungeon->weapons = malloc(weaponCount * sizeof(Weapon));
    char line[20];
    for (int i = 0; i < weaponCount && fgets(line, sizeof(line), file); i++) {
        strtok(line, "\n"); // Remove newline character
        strcpy(dungeon->weapons[i].name, line);
        dungeon->weapons[i].damage = rand() % 10 + 5;  // Random damage between 5 and 14
        dungeon->weapons[i].durability = rand() % 10 + 10; // Random durability between 10 and 19
    }
    fclose(file);
}

// Function to initialize the dungeon grid with monsters, weapons, and empty rooms
void initializeDungeon(Dungeon *dungeon) {
    dungeon->monster_count = 0;
    dungeon->weapon_count = 0;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int randVal = rand() % 10;
            if (randVal < 2) {
                dungeon->grid[i][j] = MONSTER_CHAR; // Place monster
                dungeon->monster_count++;
            } else if (randVal < 4) {
                dungeon->grid[i][j] = WEAPON_CHAR; // Place weapon
                dungeon->weapon_count++;
            } else {
                dungeon->grid[i][j] = ROOM_CHAR; // Empty room
            }
        }
    }
}

// Function to print the dungeon layout
void printDungeon(Dungeon *dungeon) {
    printf("\nDungeon Layout:\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%c ", dungeon->grid[i][j]);
        }
        printf("\n");
    }
}

// Function to place the player in a random empty room in the dungeon
void placePlayerInDungeon(Dungeon *dungeon, Player *player) {
    while (1) {
        int x = rand() % GRID_SIZE;
        int y = rand() % GRID_SIZE;
        if (dungeon->grid[x][y] == ROOM_CHAR) { // Ensure it's an empty room
            player->pos_x = x;
            player->pos_y = y;
            dungeon->grid[player->pos_x][player->pos_y] = PLAYER_CHAR; // Place player
            break;
        }
    }
}

// Function to describe all monsters and weapons present in the dungeon
void describeMonstersAndWeapons(Dungeon *dungeon) {
    printf("\nMonsters in the Dungeon:\n");
    for (int i = 0; i < dungeon->monster_count; i++) {
        printf("%d: %s (Health: %d, Damage: %d)\n", i, dungeon->monsters[i].name, dungeon->monsters[i].health, dungeon->monsters[i].damage);
    }
    printf("\nWeapons in the Dungeon:\n");
    for (int i = 0; i < dungeon->weapon_count; i++) {
        printf("%d: %s (Damage: %d, Durability: %d)\n", i, dungeon->weapons[i].name, dungeon->weapons[i].damage, dungeon->weapons[i].durability);
    }
}

// Function to handle monster attacks on the player
void monstersAttackPlayer(Dungeon *dungeon, Player *player) {
    for (int i = 0; i < dungeon->monster_count; i++) {
        if (dungeon->monsters[i].health > 0) { // If monster is alive
            player->health -= dungeon->monsters[i].damage; // Reduce player's health
            printf("%s attacked! Player health: %d\n", dungeon->monsters[i].name, player->health);
        }
    }
}

// Function to move the player in the specified direction (N, S, E, W)
void movePlayer(Dungeon *dungeon, Player *player, char direction) {
    int new_x = player->pos_x;
    int new_y = player->pos_y;
    switch (direction) {
        case 'N': new_x--; break; // Move North
        case 'S': new_x++; break; // Move South
        case 'E': new_y++; break; // Move East
        case 'W': new_y--; break; // Move West
        default: printf("Invalid move!\n"); return;
    }
    if (new_x >= 0 && new_x < GRID_SIZE && new_y >= 0 && new_y < GRID_SIZE) {
        dungeon->grid[player->pos_x][player->pos_y] = ROOM_CHAR; // Clear old position
        player->pos_x = new_x;
        player->pos_y = new_y;
        dungeon->grid[player->pos_x][player->pos_y] = PLAYER_CHAR; // Set new position
        printf("Player moved to (%d, %d).\n", player->pos_x, player->pos_y);
    } else {
        printf("Move failed: out of bounds.\n"); // Handle out-of-bounds movement
    }
    printDungeon(dungeon); // Show updated dungeon layout
    monstersAttackPlayer(dungeon, player); // Trigger monster attacks after player moves
}

// Function to check if the player can escape the dungeon from the starting point
void checkForEscape(Player *player) {
    if (player->pos_x == 0 && player->pos_y == 0) { // Check if player is at the starting point
        char action[10];
        printf("You have returned to the starting point. Type 'escape' to leave the dungeon: ");
        scanf("%s", action);
        if (strcmp(action, "escape") == 0) {
            printf("You escaped the dungeon!\n");
            printf("Monsters defeated: %d\n", player->monsters_defeated); // Display monsters defeated
            exit(0); // End the game
        }
    }
}

// Main function to run the dungeon exploration game
int main() {
    srand(time(0)); // Seed the random number generator
    Dungeon dungeon;
    Player player = { .health = 100, .monsters_defeated = 0 }; // Initialize player attributes

    initializeDungeon(&dungeon); // Set up the dungeon layout
    loadMonstersFromFile(&dungeon, dungeon.monster_count); // Load monsters into dungeon
    loadWeaponsFromFile(&dungeon, dungeon.weapon_count); // Load weapons into dungeon
    printDungeon(&dungeon); // Display the initial dungeon layout

    placePlayerInDungeon(&dungeon, &player); // Place player in the dungeon
    describeMonstersAndWeapons(&dungeon); // Describe the monsters and weapons present

    char action[10];
    while (player.health > 0) { // Game loop continues while the player is alive
        printf("\nEnter action (N/S/E/W to move, 'escape' to leave dungeon): ");
        scanf("%s", action);

        if (strcmp(action, "escape") == 0) {
            checkForEscape(&player); // Check if the player can escape the dungeon
        }

        if (strlen(action) == 1) {
            movePlayer(&dungeon, &player, action[0]); // Move player based on input
        } else {
            printf("Invalid command.\n"); // Handle invalid commands
        }
    }

    printf("Game Over!\n"); // End the game when player health reaches zero
    free(dungeon.monsters); // Free allocated memory for monsters
    free(dungeon.weapons); // Free allocated memory for weapons
    return 0; // Exit the program
}
