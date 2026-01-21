/**
 * pokemon.h - Main header file for Pokedex application
 * Contains all struct definitions and function prototypes
 */

#ifndef POKEMON_H
#define POKEMON_H

#include <stdbool.h>
#include <stddef.h>

// ============================================================================
// Data Structures
// ============================================================================

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

// ============================================================================
// File I/O Functions (file_io.c)
// ============================================================================

int load_pokemon_data(const char* filename, PokedexData* pokedex);
int load_user_progress(const char* filename, UserProgress* progress);
int save_user_progress(const char* filename, UserProgress* progress);
void initialize_progress(UserProgress* progress);

// ============================================================================
// Search Functions (search.c)
// ============================================================================

Pokemon* search_by_id(PokedexData* pokedex, int id);
Pokemon* search_by_name(PokedexData* pokedex, const char* name);
BSTNode* bst_insert(BSTNode* root, Pokemon* pokemon);
void bst_destroy(BSTNode* root);

// ============================================================================
// Progress Functions (progress.c)
// ============================================================================

void mark_encountered(UserProgress* progress, int pokemon_id);
void mark_caught(UserProgress* progress, int pokemon_id);
ProgressEntry* get_progress(UserProgress* progress, int pokemon_id);
void reset_pokemon(UserProgress* progress, int pokemon_id);
void reset_all_progress(UserProgress* progress);

// ============================================================================
// JSON Functions (json.c)
// ============================================================================

void pokemon_to_json(Pokemon* p, ProgressEntry* prog, char* buffer, size_t size);
void progress_to_json(UserProgress* progress, char* buffer, size_t size);
void list_to_json(PokedexData* pokedex, UserProgress* progress, 
                  bool caught_only, bool seen_only, char* buffer, size_t size);

// ============================================================================
// Server Functions (http_server.c)
// ============================================================================

void send_response(int client_sock, int status_code, const char* content_type, 
                   const char* body, size_t body_len);
void handle_request(int client_sock, const char* request,
                    PokedexData* pokedex, UserProgress* progress);

#endif
