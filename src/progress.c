/**
 * progress.c - User progress tracking
 * Handles marking Pokemon as encountered/caught
 */

#include "../include/pokemon.h"

/**
 * Mark a Pokemon as encountered
 */
void mark_encountered(UserProgress* progress, int pokemon_id) {
    if (pokemon_id < 1 || pokemon_id > 151) return;
    
    ProgressEntry* entry = &progress->entries[pokemon_id - 1];
    if (!entry->encountered) {
        entry->encountered = true;
        progress->total_encountered++;
    }
}

/**
 * Mark a Pokemon as caught (also marks as encountered)
 */
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

/**
 * Get progress entry for a specific Pokemon
 */
ProgressEntry* get_progress(UserProgress* progress, int pokemon_id) {
    if (pokemon_id < 1 || pokemon_id > 151) return NULL;
    return &progress->entries[pokemon_id - 1];
}

/**
 * Reset a specific Pokemon's progress (remove caught and seen status)
 */
void reset_pokemon(UserProgress* progress, int pokemon_id) {
    if (pokemon_id < 1 || pokemon_id > 151) return;
    
    ProgressEntry* entry = &progress->entries[pokemon_id - 1];
    
    if (entry->caught) {
        entry->caught = false;
        progress->total_caught--;
    }
    
    if (entry->encountered) {
        entry->encountered = false;
        progress->total_encountered--;
    }
}

/**
 * Reset all Pokemon progress
 */
void reset_all_progress(UserProgress* progress) {
    for (int i = 0; i < 151; i++) {
        progress->entries[i].encountered = false;
        progress->entries[i].caught = false;
    }
    progress->total_encountered = 0;
    progress->total_caught = 0;
}
