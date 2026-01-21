// pokemon.h
#ifndef POKEMON_H
#define POKEMON_H

#include <stdbool.h>

typedef struct {
    int id;
    char name[30];
    char type1[15];
    char type2[15];
    int hp;
    int attack;
    int defense;
    int sp_attack;
    int sp_defense;
    int speed;
    char ability1[30];
    char ability2[30];
    char description[256];
} Pokemon;

typedef struct BSTNode {
    Pokemon* pokemon;
    struct BSTNode* left;
    struct BSTNode* right;
} BSTNode;

typedef struct {
    Pokemon pokemon_array[151];
    BSTNode* name_bst_root;
    int count;
} PokedexData;

typedef struct {
    int id;
    bool encountered;
    bool caught;
} ProgressEntry;

typedef struct {
    ProgressEntry entries[151];
    int total_encountered;
    int total_caught;
} UserProgress;

// Function prototypes
int load_pokemon_data(const char* filename, PokedexData* pokedex);
int load_user_progress(const char* filename, UserProgress* progress);
int save_user_progress(const char* filename, UserProgress* progress);
void initialize_progress(UserProgress* progress);

Pokemon* search_by_id(PokedexData* pokedex, int id);
Pokemon* search_by_name(PokedexData* pokedex, const char* name);

void mark_encountered(UserProgress* progress, int pokemon_id);
void mark_caught(UserProgress* progress, int pokemon_id);
ProgressEntry* get_progress(UserProgress* progress, int pokemon_id);

void display_pokemon_details(Pokemon* p, ProgressEntry* prog);
void display_pokemon_summary(Pokemon* p, ProgressEntry* prog);
void display_statistics(UserProgress* progress);
void list_all_pokemon(PokedexData* pokedex, UserProgress* progress, 
                      bool caught_only, bool encountered_only);

BSTNode* bst_insert(BSTNode* root, Pokemon* pokemon);
void bst_destroy(BSTNode* root);

#endif

// ============================================================================
// file_io.c - File I/O operations
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Remove quotes and clean CSV field
void clean_csv_field(char* dest, const char* src, size_t max_len) {
    size_t j = 0;
    bool in_quotes = false;
    
    for (size_t i = 0; src[i] != '\0' && j < max_len - 1; i++) {
        if (src[i] == '"') {
            in_quotes = !in_quotes;
        } else if (!in_quotes) {
            dest[j++] = src[i];
        } else {
            dest[j++] = src[i];
        }
    }
    dest[j] = '\0';
    
    // Trim trailing whitespace
    while (j > 0 && isspace((unsigned char)dest[j-1])) {
        dest[--j] = '\0';
    }
}

int load_pokemon_data(const char* filename, PokedexData* pokedex) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open %s\n", filename);
        return 0;
    }
    
    char line[1024];
    // Skip header line
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return 0;
    }
    
    pokedex->count = 0;
    pokedex->name_bst_root = NULL;
    
    while (fgets(line, sizeof(line), file) && pokedex->count < 151) {
        Pokemon* p = &pokedex->pokemon_array[pokedex->count];
        
        char* token = strtok(line, ",");
        if (!token) continue;
        
        // Parse CSV fields
        p->id = atoi(token);
        
        token = strtok(NULL, ",");
        clean_csv_field(p->name, token, sizeof(p->name));
        
        token = strtok(NULL, ",");
        clean_csv_field(p->type1, token, sizeof(p->type1));
        
        token = strtok(NULL, ",");
        clean_csv_field(p->type2, token, sizeof(p->type2));
        
        token = strtok(NULL, ",");
        p->hp = atoi(token);
        
        token = strtok(NULL, ",");
        p->attack = atoi(token);
        
        token = strtok(NULL, ",");
        p->defense = atoi(token);
        
        token = strtok(NULL, ",");
        p->sp_attack = atoi(token);
        
        token = strtok(NULL, ",");
        p->sp_defense = atoi(token);
        
        token = strtok(NULL, ",");
        p->speed = atoi(token);
        
        token = strtok(NULL, ",");
        clean_csv_field(p->ability1, token, sizeof(p->ability1));
        
        token = strtok(NULL, ",");
        clean_csv_field(p->ability2, token, sizeof(p->ability2));
        
        // Rest of line is description
        token = strtok(NULL, "\n");
        if (token) {
            clean_csv_field(p->description, token, sizeof(p->description));
        } else {
            p->description[0] = '\0';
        }
        
        // Insert into BST
        pokedex->name_bst_root = bst_insert(pokedex->name_bst_root, p);
        pokedex->count++;
    }
    
    fclose(file);
    printf("Loaded %d Pokémon from database.\n", pokedex->count);
    return pokedex->count;
}

void initialize_progress(UserProgress* progress) {
    for (int i = 0; i < 151; i++) {
        progress->entries[i].id = i + 1;
        progress->entries[i].encountered = false;
        progress->entries[i].caught = false;
    }
    progress->total_encountered = 0;
    progress->total_caught = 0;
}

int load_user_progress(const char* filename, UserProgress* progress) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        // File doesn't exist, create new progress
        initialize_progress(progress);
        return 1;
    }
    
    size_t read = fread(progress, sizeof(UserProgress), 1, file);
    fclose(file);
    
    if (read != 1) {
        initialize_progress(progress);
        return 0;
    }
    
    return 1;
}

int save_user_progress(const char* filename, UserProgress* progress) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error: Could not save progress to %s\n", filename);
        return 0;
    }
    
    size_t written = fwrite(progress, sizeof(UserProgress), 1, file);
    fclose(file);
    
    return (written == 1);
}

// ============================================================================
// search.c - Search functionality
// ============================================================================

#include <strings.h>

Pokemon* search_by_id(PokedexData* pokedex, int id) {
    if (id < 1 || id > 151) {
        return NULL;
    }
    return &pokedex->pokemon_array[id - 1];
}

BSTNode* bst_search(BSTNode* root, const char* name) {
    if (root == NULL) return NULL;
    
    int cmp = strcasecmp(name, root->pokemon->name);
    
    if (cmp == 0) {
        return root;
    } else if (cmp < 0) {
        return bst_search(root->left, name);
    } else {
        return bst_search(root->right, name);
    }
}

Pokemon* search_by_name(PokedexData* pokedex, const char* name) {
    BSTNode* node = bst_search(pokedex->name_bst_root, name);
    return (node != NULL) ? node->pokemon : NULL;
}

BSTNode* bst_insert(BSTNode* root, Pokemon* pokemon) {
    if (root == NULL) {
        BSTNode* new_node = (BSTNode*)malloc(sizeof(BSTNode));
        new_node->pokemon = pokemon;
        new_node->left = NULL;
        new_node->right = NULL;
        return new_node;
    }
    
    int cmp = strcasecmp(pokemon->name, root->pokemon->name);
    
    if (cmp < 0) {
        root->left = bst_insert(root->left, pokemon);
    } else if (cmp > 0) {
        root->right = bst_insert(root->right, pokemon);
    }
    
    return root;
}

void bst_destroy(BSTNode* root) {
    if (root == NULL) return;
    bst_destroy(root->left);
    bst_destroy(root->right);
    free(root);
}

// ============================================================================
// progress.c - Progress tracking
// ============================================================================

void mark_encountered(UserProgress* progress, int pokemon_id) {
    if (pokemon_id < 1 || pokemon_id > 151) return;
    
    ProgressEntry* entry = &progress->entries[pokemon_id - 1];
    if (!entry->encountered) {
        entry->encountered = true;
        progress->total_encountered++;
    }
}

void mark_caught(UserProgress* progress, int pokemon_id) {
    if (pokemon_id < 1 || pokemon_id > 151) return;
    
    ProgressEntry* entry = &progress->entries[pokemon_id - 1];
    
    if (!entry->encountered) {
        entry->encountered = true;
        progress->total_encountered++;
    }
    
    if (!entry->caught) {
        entry->caught = true;
        progress->total_caught++;
    }
}

ProgressEntry* get_progress(UserProgress* progress, int pokemon_id) {
    if (pokemon_id < 1 || pokemon_id > 151) return NULL;
    return &progress->entries[pokemon_id - 1];
}

// ============================================================================
// display.c - Display functions
// ============================================================================

void print_separator() {
    printf("════════════════════════════════════════════════════════════════\n");
}

void print_stat_bar(const char* label, int value, int max_value) {
    printf("  %-12s %3d  ", label, value);
    int bars = (value * 20) / max_value;
    for (int i = 0; i < 20; i++) {
        printf(i < bars ? "#" : "-");
    }
    printf("\n");
}

void display_pokemon_details(Pokemon* p, ProgressEntry* prog) {
    print_separator();
    printf("  #%03d - %s\n", p->id, p->name);
    print_separator();
    
    // Type
    printf("  Type: %s", p->type1);
    if (strlen(p->type2) > 0) {
        printf("/%s", p->type2);
    }
    printf("\n");
    
    // Status
    printf("  Status: ");
    if (prog && prog->caught) {
        printf("* CAUGHT\n");
    } else if (prog && prog->encountered) {
        printf("o SEEN\n");
    } else {
        printf("- UNKNOWN\n");
    }
    
    print_separator();
    printf("  BASE STATS\n");
    print_stat_bar("HP:", p->hp, 255);
    print_stat_bar("Attack:", p->attack, 255);
    print_stat_bar("Defense:", p->defense, 255);
    print_stat_bar("Sp. Atk:", p->sp_attack, 255);
    print_stat_bar("Sp. Def:", p->sp_defense, 255);
    print_stat_bar("Speed:", p->speed, 255);
    
    print_separator();
    printf("  ABILITIES\n");
    printf("  - %s\n", p->ability1);
    if (strlen(p->ability2) > 0) {
        printf("  - %s\n", p->ability2);
    }
    
    print_separator();
    printf("  DESCRIPTION\n  %s\n", p->description);
    print_separator();
}

void display_pokemon_summary(Pokemon* p, ProgressEntry* prog) {
    const char* status = " ";
    if (prog) {
        if (prog->caught) status = "*";
        else if (prog->encountered) status = "o";
    }
    
    printf("  [%s] #%03d %-12s  %-8s", status, p->id, p->name, p->type1);
    if (strlen(p->type2) > 0) {
        printf("/%-8s", p->type2);
    }
    printf("\n");
}

void display_statistics(UserProgress* progress) {
    print_separator();
    printf("  POKÉDEX STATISTICS\n");
    print_separator();
    
    printf("  Pokémon Seen:   %3d / 151 (%5.1f%%)\n", 
           progress->total_encountered, 
           (progress->total_encountered / 151.0) * 100);
    
    printf("  Pokémon Caught: %3d / 151 (%5.1f%%)\n", 
           progress->total_caught, 
           (progress->total_caught / 151.0) * 100);
    
    print_separator();
    
    // Progress bar
    printf("  Overall Progress: [");
    int bars = (progress->total_caught * 30) / 151;
    for (int i = 0; i < 30; i++) {
        printf(i < bars ? "#" : "-");
    }
    printf("]\n");
    print_separator();
}

void list_all_pokemon(PokedexData* pokedex, UserProgress* progress, 
                      bool caught_only, bool encountered_only) {
    print_separator();
    if (caught_only) {
        printf("  CAUGHT POKÉMON\n");
    } else if (encountered_only) {
        printf("  ENCOUNTERED POKÉMON\n");
    } else {
        printf("  ALL POKÉMON\n");
    }
    print_separator();
    
    int count = 0;
    for (int i = 0; i < pokedex->count; i++) {
        Pokemon* p = &pokedex->pokemon_array[i];
        ProgressEntry* prog = get_progress(progress, p->id);
        
        bool show = true;
        if (caught_only && (!prog || !prog->caught)) show = false;
        if (encountered_only && (!prog || !prog->encountered)) show = false;
        
        if (show) {
            display_pokemon_summary(p, prog);
            count++;
        }
    }
    
    print_separator();
    printf("  Total: %d Pokémon\n", count);
    print_separator();
}

// ============================================================================
// main.c - Main program
// ============================================================================

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void display_main_menu(UserProgress* progress) {
    printf("\n");
    print_separator();
    printf("            KANTO POKÉDEX v1.0\n");
    print_separator();
    printf("  Progress: %d/151 Seen | %d/151 Caught\n", 
           progress->total_encountered, progress->total_caught);
    print_separator();
    printf("  1. Search by Dex Number\n");
    printf("  2. Search by Name\n");
    printf("  3. List All Pokémon\n");
    printf("  4. List Caught Pokémon\n");
    printf("  5. List Encountered Pokémon\n");
    printf("  6. View Statistics\n");
    printf("  7. Exit\n");
    print_separator();
    printf("  Enter choice: ");
}

void handle_pokemon_actions(Pokemon* p, UserProgress* progress) {
    ProgressEntry* prog = get_progress(progress, p->id);
    display_pokemon_details(p, prog);
    
    printf("\n  1. Mark as Encountered\n");
    printf("  2. Mark as Caught\n");
    printf("  3. Back to Menu\n");
    printf("  Enter choice: ");
    
    int choice;
    if (scanf("%d", &choice) != 1) {
        clear_input_buffer();
        return;
    }
    clear_input_buffer();
    
    switch (choice) {
        case 1:
            mark_encountered(progress, p->id);
            printf("\n  + Marked as encountered!\n");
            break;
        case 2:
            mark_caught(progress, p->id);
            printf("\n  ✓ Marked as caught!\n");
            break;
        case 3:
            return;
    }
}

int main() {
    PokedexData pokedex;
    UserProgress progress;
    
    // Load data
    if (!load_pokemon_data("pokemon_data.csv", &pokedex)) {
        printf("Failed to load Pokémon data!\n");
        return 1;
    }
    
    load_user_progress("user_progress.dat", &progress);
    
    int running = 1;
    while (running) {
        display_main_menu(&progress);
        
        int choice;
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("Invalid input!\n");
            continue;
        }
        clear_input_buffer();
        
        switch (choice) {
            case 1: {
                printf("\n  Enter Dex Number (1-151): ");
                int id;
                if (scanf("%d", &id) != 1) {
                    clear_input_buffer();
                    printf("  Invalid input!\n");
                    break;
                }
                clear_input_buffer();
                
                Pokemon* p = search_by_id(&pokedex, id);
                if (p) {
                    handle_pokemon_actions(p, &progress);
                    save_user_progress("user_progress.dat", &progress);
                } else {
                    printf("  Pokémon not found!\n");
                }
                break;
            }
            
            case 2: {
                printf("\n  Enter Pokémon Name: ");
                char name[30];
                if (!fgets(name, sizeof(name), stdin)) {
                    printf("  Invalid input!\n");
                    break;
                }
                name[strcspn(name, "\n")] = 0;
                
                Pokemon* p = search_by_name(&pokedex, name);
                if (p) {
                    handle_pokemon_actions(p, &progress);
                    save_user_progress("user_progress.dat", &progress);
                } else {
                    printf("  Pokémon not found!\n");
                }
                break;
            }
            
            case 3:
                list_all_pokemon(&pokedex, &progress, false, false);
                break;
            
            case 4:
                list_all_pokemon(&pokedex, &progress, true, false);
                break;
            
            case 5:
                list_all_pokemon(&pokedex, &progress, false, true);
                break;
            
            case 6:
                display_statistics(&progress);
                break;
            
            case 7:
                save_user_progress("user_progress.dat", &progress);
                printf("\n  Progress saved. Goodbye, Trainer!\n\n");
                running = 0;
                break;
            
            default:
                printf("  Invalid choice!\n");
        }
    }
    
    // Cleanup
    bst_destroy(pokedex.name_bst_root);
    
    return 0;
}