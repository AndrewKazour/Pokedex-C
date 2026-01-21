/**
 * file_io.c - File I/O operations
 * Handles CSV parsing and user progress file operations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/pokemon.h"

// Forward declaration for BST insert
extern BSTNode* bst_insert(BSTNode* root, Pokemon* pokemon);

/**
 * Parse next CSV field, handling quotes and empty fields
 * Returns pointer to next position after the field, or NULL if end of line
 */
static char* parse_csv_field(char* input, char* output, size_t max_len) {
    if (input == NULL || *input == '\0' || *input == '\n' || *input == '\r') {
        output[0] = '\0';
        return NULL;
    }
    
    size_t j = 0;
    bool in_quotes = false;
    char* p = input;
    
    // Check if field starts with quote
    if (*p == '"') {
        in_quotes = true;
        p++;
    }
    
    while (*p != '\0' && *p != '\n' && *p != '\r') {
        if (in_quotes) {
            if (*p == '"') {
                // Check for escaped quote ""
                if (*(p + 1) == '"') {
                    if (j < max_len - 1) output[j++] = '"';
                    p += 2;
                    continue;
                } else {
                    // End of quoted field
                    in_quotes = false;
                    p++;
                    continue;
                }
            }
        } else {
            if (*p == ',') {
                // End of field
                p++;  // Skip the comma
                break;
            }
        }
        
        if (j < max_len - 1) {
            output[j++] = *p;
        }
        p++;
    }
    
    output[j] = '\0';
    
    // Trim trailing whitespace
    while (j > 0 && isspace((unsigned char)output[j-1])) {
        output[--j] = '\0';
    }
    
    // If we hit end of line, return NULL
    if (*p == '\0' || *p == '\n' || *p == '\r') {
        return NULL;
    }
    
    return p;
}

/**
 * Load Pokemon data from CSV file
 * Returns number of Pokemon loaded, or 0 on failure
 */
int load_pokemon_data(const char* filename, PokedexData* pokedex) {
    printf("Attempting to open: %s\n", filename);
    fflush(stdout);
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open %s\n", filename);
        fflush(stdout);
        return 0;
    }
    printf("File opened successfully\n");
    fflush(stdout);
    
    char line[1024];
    if (!fgets(line, sizeof(line), file)) {
        printf("Error: Could not read header line\n");
        fflush(stdout);
        fclose(file);
        return 0;
    }
    
    pokedex->count = 0;
    pokedex->name_bst_root = NULL;
    
    while (fgets(line, sizeof(line), file) && pokedex->count < 151) {
        Pokemon* p = &pokedex->pokemon_array[pokedex->count];
        char field[256];
        char* ptr = line;
        
        // id
        ptr = parse_csv_field(ptr, field, sizeof(field));
        p->id = atoi(field);
        if (p->id == 0) continue;  // Skip invalid lines
        
        // name
        ptr = parse_csv_field(ptr, p->name, sizeof(p->name));
        
        // type1
        ptr = parse_csv_field(ptr, p->type1, sizeof(p->type1));
        
        // type2 (can be empty)
        ptr = parse_csv_field(ptr, p->type2, sizeof(p->type2));
        
        // hp
        ptr = parse_csv_field(ptr, field, sizeof(field));
        p->hp = atoi(field);
        
        // attack
        ptr = parse_csv_field(ptr, field, sizeof(field));
        p->attack = atoi(field);
        
        // defense
        ptr = parse_csv_field(ptr, field, sizeof(field));
        p->defense = atoi(field);
        
        // sp_attack
        ptr = parse_csv_field(ptr, field, sizeof(field));
        p->sp_attack = atoi(field);
        
        // sp_defense
        ptr = parse_csv_field(ptr, field, sizeof(field));
        p->sp_defense = atoi(field);
        
        // speed
        ptr = parse_csv_field(ptr, field, sizeof(field));
        p->speed = atoi(field);
        
        // ability1
        ptr = parse_csv_field(ptr, p->ability1, sizeof(p->ability1));
        
        // ability2 (can be empty)
        ptr = parse_csv_field(ptr, p->ability2, sizeof(p->ability2));
        
        // description (rest of line)
        if (ptr) {
            parse_csv_field(ptr, p->description, sizeof(p->description));
        } else {
            p->description[0] = '\0';
        }
        
        pokedex->name_bst_root = bst_insert(pokedex->name_bst_root, p);
        pokedex->count++;
    }
    
    fclose(file);
    printf("Loaded %d Pokemon\n", pokedex->count);
    fflush(stdout);
    return pokedex->count > 0 ? pokedex->count : 1;
}

/**
 * Initialize user progress to default values
 */
void initialize_progress(UserProgress* progress) {
    for (int i = 0; i < 151; i++) {
        progress->entries[i].id = i + 1;
        progress->entries[i].encountered = false;
        progress->entries[i].caught = false;
    }
    progress->total_encountered = 0;
    progress->total_caught = 0;
}

/**
 * Load user progress from binary file
 * Returns 1 on success, 0 on failure (initializes default progress on failure)
 */
int load_user_progress(const char* filename, UserProgress* progress) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
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

/**
 * Save user progress to binary file
 * Returns 1 on success, 0 on failure
 */
int save_user_progress(const char* filename, UserProgress* progress) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        return 0;
    }
    
    size_t written = fwrite(progress, sizeof(UserProgress), 1, file);
    fclose(file);
    
    return (written == 1);
}
