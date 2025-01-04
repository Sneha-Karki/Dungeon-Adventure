/****************************************** 
-Name: Sneha Karki
-Fall 2024, CS2713 <003>
-dwx106
-Project 2 Part 2: Sauron-sama's Dungeon- Filling Dungeons
******************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define GRID_SIZE 10
#define ROOM_CHAR '*'
#define EMPTY_CHAR '-'
#define MONSTER_CHAR 'M'
#define WEAPON_CHAR 'W'
#define POTION_CHAR 'P'

typedef struct {char name[20];int health;char weapon[20];  // Monster's weapon
} Monster;

typedef struct {char name[20];int damage;int durability;
} Weapon;

typedef struct {char effect[20];int power;        
} Potion;

typedef struct {char grid[GRID_SIZE][GRID_SIZE];Monster* monsters;int monster_count;Weapon* weapons;int weapon_count;Potion* potions;int potion_count;
} Dungeon;

void loadNamesFromFile(const char* filename, char names[][20], int* count) {
    FILE* file = fopen(filename, "r");
    char line[20];
    *count = 0;
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;  // Remove newline character
        strcpy(names[*count], line);
        (*count)++;
    }

    fclose(file);
}

void initializeDungeon(Dungeon* dungeon) {
    dungeon->monster_count = 0;
    dungeon->weapon_count = 0;
    dungeon->potion_count = 0;

    // Dynamically allocate memory for monsters, weapons, and potions
    dungeon->monsters = malloc(sizeof(Monster) * 100);  // Arbitrary size of 100 for now
    dungeon->weapons = malloc(sizeof(Weapon) * 100);  // Arbitrary size of 100
    dungeon->potions = malloc(sizeof(Potion) * 100);  // Arbitrary size of 100

    if (!dungeon->monsters || !dungeon->weapons || !dungeon->potions) {
        printf("Memory allocation failed!\n");
        exit(1);  // Exit if memory allocation fails
    }

    // Initialize the dungeon grid with rooms and empty spaces
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (rand() % 2) {
                dungeon->grid[i][j] = ROOM_CHAR; // Randomly assign rooms
            } else {
                dungeon->grid[i][j] = EMPTY_CHAR; // Empty spaces
            }
        }
    }
}

int getNextFileNumber() {
    FILE* file = fopen("file_number.txt", "r");
    int lastNumber = 0;

    if (file) {
        fscanf(file, "%d", &lastNumber);
        fclose(file);
    }

    // Increment the file number
    lastNumber++;

    // Save the updated file number
    file = fopen("file_number.txt", "w");
    if (file) {
        fprintf(file, "%d", lastNumber);
        fclose(file);
    }

    return lastNumber;
}

void printDungeonToFile(Dungeon* dungeon, int fileNumber) {
    char filename[20];
    snprintf(filename, sizeof(filename), "%d.txt", fileNumber);
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    // Write the monsters' details to the file
    for (int i = 0; i < dungeon->monster_count; i++) {
        fprintf(file, "\n(Monster: %s, Health------> %d, Montser's Weapon------> %s)\n\n",
                dungeon->monsters[i].name, dungeon->monsters[i].health, dungeon->monsters[i].weapon);
    }

    // Write the weapons' details to the file
    for (int i = 0; i < dungeon->weapon_count; i++) {
        fprintf(file, "\n(Weapon found at Dungeon: %s,Damage------> %d,Durability------> %d)\n",
                dungeon->weapons[i].name, dungeon->weapons[i].damage, dungeon->weapons[i].durability);
    }

    // Write the potions' details to the file
    for (int i = 0; i < dungeon->potion_count; i++) {
        fprintf(file, "\n(Potion(%s), Power------> %d)\n",
                dungeon->potions[i].effect, dungeon->potions[i].power);
    }

    // Write the dungeon layout to the file
    fprintf(file, "\nYour creation:\n");
    fprintf(file, "(* = Room, - = empty, M = Monster, W = Weapon, P = Potion)\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            fprintf(file, "%c ", dungeon->grid[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);

    // Display the name of the output file
    printf("Dungeon details saved to: %s\n", filename);
}

void printDungeonToConsole(Dungeon* dungeon) {
    // Display the dungeon layout on the console
    printf("\nDungeon Layout:\n");
    printf("(* = Room, - = empty, M = Monster, W = Weapon, P = Potion)\n");
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%c ", dungeon->grid[i][j]);
        }
        printf("\n");
    }
}

void loadDungeonFromFile(int fileNumber) {
    char filename[20];
    snprintf(filename, sizeof(filename), "%d.txt", fileNumber);

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    char line[256];
    printf("\nDungeon details from %s:\n", filename);
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}

void addRandomMonster(Dungeon* dungeon, char monsterNames[][20], int monsterCount, char weaponNames[][20], int weaponCount) {
    int row = rand() % GRID_SIZE;
    int column = rand() % GRID_SIZE;
    int randomIndexMonster = rand() % monsterCount;
    int randomIndexWeapon = rand() % weaponCount;

    // Check if the grid spot is a room and empty
    if (dungeon->grid[row][column] == ROOM_CHAR) {
        Monster* monster = &dungeon->monsters[dungeon->monster_count];
        strcpy(monster->name, monsterNames[randomIndexMonster]);
        monster->health = rand() % 11 + 10;  // Random health between 10-20
        strcpy(monster->weapon, weaponNames[randomIndexWeapon]); // Assign random weapon from weapons list

        dungeon->monster_count++;
        dungeon->grid[row][column] = MONSTER_CHAR; // Mark the monster's position on the grid

        printf("\n(Monster: %s, Health------> %d, Monster's Weapon------> %s)\n\n", monster->name, monster->health, monster->weapon);
    }
}

void addRandomWeapon(Dungeon* dungeon, char weaponNames[][20], int weaponCount) {
    int row = rand() % GRID_SIZE;
    int column = rand() % GRID_SIZE;
    int randomIndex = rand() % weaponCount;

    // Check if the grid spot is a room and empty
    if (dungeon->grid[row][column] == ROOM_CHAR) {
        Weapon* weapon = &dungeon->weapons[dungeon->weapon_count];
        strcpy(weapon->name, weaponNames[randomIndex]);
        weapon->damage = rand() % 10 + 1;     // Random damage between 1-10
        weapon->durability = rand() % 21 + 10; // Random durability between 10-30

        dungeon->weapon_count++;
        dungeon->grid[row][column] = WEAPON_CHAR; // Mark the weapon's position on the grid

        printf("\n(Weapon found at Dungeon: %s,Damage------> %d,Durability------> %d)\n", weapon->name, weapon->damage, weapon->durability);
    }
}

void addRandomPotion(Dungeon* dungeon) {
    int row = rand() % GRID_SIZE;
    int column = rand() % GRID_SIZE;

    // Check if the grid spot is a room and empty
    if (dungeon->grid[row][column] == ROOM_CHAR) {
        Potion* potion = &dungeon->potions[dungeon->potion_count];
        strcpy(potion->effect, rand() % 2 == 0 ? "Healing" : "Repairing");
        potion->power = rand() % 10 + 1; // Random power between 1-10

        dungeon->potion_count++;
        dungeon->grid[row][column] = POTION_CHAR; // Mark the potion's position on the grid

        printf("\n(Potion(%s), Power------> %d)\n", potion->effect, potion->power);
    }
}

int main() {
    srand(time(0)); // Seed for randomness
    char action[10];
    printf("\n☠︎︎ New orders from Sauron-sama has been received! ☠︎︎\n");
    printf("☠︎︎ ☠︎︎ You are to fill the dungeons with items(weapons, potions) and his fav monsters ☠︎︎ ☠︎︎\n");
    printf("☠︎︎ ☠︎︎ ☠︎︎ Use your magical power to fill dungeons with items and monsters ☠︎︎ ☠︎︎ ☠︎︎\n");
    printf("\n☠︎︎ ☠︎︎ ☠︎︎ ☠︎︎ To generate enter 'gen', to load enter 'load'☠︎︎ ☠︎︎ ☠︎︎ ☠︎︎\n[Magical words only work on lowercase!]:");
    scanf("%s", action);

    if (strcmp(action, "gen") == 0) {
        char monsterNames[100][20];
        char weaponNames[100][20];
        int monsterCount = 0, weaponCount = 0;
        // Load the monster and weapon names
        loadNamesFromFile("monsters.txt", monsterNames, &monsterCount);
        loadNamesFromFile("weapons.txt", weaponNames, &weaponCount);

        Dungeon dungeon;
        initializeDungeon(&dungeon);
        addRandomMonster(&dungeon, monsterNames, monsterCount, weaponNames, weaponCount);
        addRandomWeapon(&dungeon, weaponNames, weaponCount);
        addRandomPotion(&dungeon);
        int fileNumber = getNextFileNumber();
        printDungeonToFile(&dungeon, fileNumber);
        printDungeonToConsole(&dungeon);
    } else if (strcmp(action, "load") == 0) {
        int fileNumber;
        printf("Please enter the file number to load (e.g., 1): ");
        scanf("%d", &fileNumber);
        loadDungeonFromFile(fileNumber);
    } else {
        printf("Double check the spelling & lowercase. Please enter 'gen' or 'load'.\n");
    }
   return 0;
}
