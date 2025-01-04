/****************************************** 
-Name: Sneha Karki
-Fall 2024, CS2713 <003>
-dwx106
-Project 2 Part 3: Sauron-sama's Dungeon- Explore the Dungeon
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

typedef struct {
    char name[20];
    int health;
    int damage;
} Monster;

typedef struct {
    char name[20];
    int damage;
    int durability;
} Weapon;

typedef struct {
    char grid[GRID_SIZE][GRID_SIZE];
    Monster *monsters;
    int monster_count;
    Weapon *weapons;
    int weapon_count;
} Dungeon;

typedef struct {
    int health;
    int pos_x, pos_y;
    int monsters_defeated;
} Player;

// Load monsters from monsters.txt
void loadMonstersFromFile(Dungeon *dungeon, int monsterCount) {
    FILE *file = fopen("monsters.txt", "r");
    if (!file) {
        printf("Error opening monsters.txt\n");
        exit(1);
    }
    dungeon->monsters = malloc(monsterCount * sizeof(Monster));
    char line[20];
    for (int i = 0; i < monsterCount && fgets(line, sizeof(line), file); i++) {
        strtok(line, "\n");
        strcpy(dungeon->monsters[i].name, line);
        dungeon->monsters[i].health = rand() % 30 + 20;
        dungeon->monsters[i].damage = rand() % 10 + 5;
    }
    fclose(file);
}

// Load weapons from weapons.txt
void loadWeaponsFromFile(Dungeon *dungeon, int weaponCount) {
    FILE *file = fopen("weapons.txt", "r");
    if (!file) {
        printf("Error opening weapons.txt\n");
        exit(1);
    }
    dungeon->weapons = malloc(weaponCount * sizeof(Weapon));
    char line[20];
    for (int i = 0; i < weaponCount && fgets(line, sizeof(line), file); i++) {
        strtok(line, "\n");
        strcpy(dungeon->weapons[i].name, line);
        dungeon->weapons[i].damage = rand() % 10 + 5;
        dungeon->weapons[i].durability = rand() % 10 + 10;
    }
    fclose(file);
}

// Initialize dungeon with M, W, and empty rooms
void initializeDungeon(Dungeon *dungeon) {
    dungeon->monster_count = 0;
    dungeon->weapon_count = 0;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            int randVal = rand() % 10;
            if (randVal < 2) {
                dungeon->grid[i][j] = MONSTER_CHAR;
                dungeon->monster_count++;
            } else if (randVal < 4) {
                dungeon->grid[i][j] = WEAPON_CHAR;
                dungeon->weapon_count++;
            } else {
                dungeon->grid[i][j] = ROOM_CHAR;
            }
        }
    }
}

// Print dungeon grid
void printDungeon(Dungeon *dungeon) {
    printf("\nDungeon Layout:\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%c ", dungeon->grid[i][j]);
        }
        printf("\n");
    }
}

// Place player in a random room
void placePlayerInDungeon(Dungeon *dungeon, Player *player) {
    while (1) {
        int x = rand() % GRID_SIZE;
        int y = rand() % GRID_SIZE;
        if (dungeon->grid[x][y] == ROOM_CHAR) {
            player->pos_x = x;
            player->pos_y = y;
            dungeon->grid[player->pos_x][player->pos_y] = PLAYER_CHAR;
            break;
        }
    }
}

// Describe monsters and weapons
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

// Monster attack function
void monstersAttackPlayer(Dungeon *dungeon, Player *player) {
    for (int i = 0; i < dungeon->monster_count; i++) {
        if (dungeon->monsters[i].health > 0) {
            player->health -= dungeon->monsters[i].damage;
            printf("%s attacked! Player health: %d\n", dungeon->monsters[i].name, player->health);
        }
    }
}

// Move player in specified direction
void movePlayer(Dungeon *dungeon, Player *player, char direction) {
    int new_x = player->pos_x;
    int new_y = player->pos_y;
    switch (direction) {
        case 'N': new_x--; break;
        case 'S': new_x++; break;
        case 'E': new_y++; break;
        case 'W': new_y--; break;
        default: printf("Invalid move!\n"); return;
    }
    if (new_x >= 0 && new_x < GRID_SIZE && new_y >= 0 && new_y < GRID_SIZE) {
        dungeon->grid[player->pos_x][player->pos_y] = ROOM_CHAR;
        player->pos_x = new_x;
        player->pos_y = new_y;
        dungeon->grid[player->pos_x][player->pos_y] = PLAYER_CHAR;
        printf("Player moved to (%d, %d).\n", player->pos_x, player->pos_y);
    } else {
        printf("Move failed: out of bounds.\n");
    }
    printDungeon(dungeon);
    monstersAttackPlayer(dungeon, player);
}

// Allow the player to escape if they return to the starting point
void checkForEscape(Player *player) {
    if (player->pos_x == 0 && player->pos_y == 0) {
        char action[10];
        printf("You have returned to the starting point. Type 'escape' to leave the dungeon: ");
        scanf("%s", action);
        if (strcmp(action, "escape") == 0) {
            printf("You escaped the dungeon!\n");
            printf("Monsters defeated: %d\n", player->monsters_defeated);
            exit(0);
        }
    }
}

int main() {
    srand(time(0));
    Dungeon dungeon;
    Player player = { .health = 100, .monsters_defeated = 0 };

    initializeDungeon(&dungeon);
    loadMonstersFromFile(&dungeon, dungeon.monster_count);
    loadWeaponsFromFile(&dungeon, dungeon.weapon_count);
    printDungeon(&dungeon);

    placePlayerInDungeon(&dungeon, &player);
    describeMonstersAndWeapons(&dungeon);

    char action[10];
    while (player.health > 0) {
        printf("\nEnter action (N/S/E/W to move, 'escape' to leave dungeon): ");
        scanf("%s", action);

        if (strcmp(action, "escape") == 0) {
            checkForEscape(&player);
        }

        if (strlen(action) == 1) {
            movePlayer(&dungeon, &player, action[0]);
        } else {
            printf("Invalid command.\n");
        }
    }

    printf("Game Over!\n");
    free(dungeon.monsters);
    free(dungeon.weapons);
    return 0;
}
