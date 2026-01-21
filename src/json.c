/**
 * json.c - JSON generation for API responses
 * Converts data structures to JSON format
 */

#include <stdio.h>
#include <string.h>
#include "../include/pokemon.h"

/**
 * Escape special characters in JSON strings
 */
static void escape_json_string(const char* src, char* dest, size_t max_len) {
    size_t j = 0;
    for (size_t i = 0; src[i] != '\0' && j < max_len - 2; i++) {
        if (src[i] == '"' || src[i] == '\\') {
            dest[j++] = '\\';
        }
        dest[j++] = src[i];
    }
    dest[j] = '\0';
}

/**
 * Convert a Pokemon to JSON format
 */
void pokemon_to_json(Pokemon* p, ProgressEntry* prog, char* buffer, size_t size) {
    char desc_escaped[512];
    escape_json_string(p->description, desc_escaped, sizeof(desc_escaped));
    
    snprintf(buffer, size,
        "{"
        "\"id\":%d,"
        "\"name\":\"%s\","
        "\"type1\":\"%s\","
        "\"type2\":\"%s\","
        "\"hp\":%d,"
        "\"attack\":%d,"
        "\"defense\":%d,"
        "\"sp_attack\":%d,"
        "\"sp_defense\":%d,"
        "\"speed\":%d,"
        "\"ability1\":\"%s\","
        "\"ability2\":\"%s\","
        "\"description\":\"%s\","
        "\"encountered\":%s,"
        "\"caught\":%s"
        "}",
        p->id, p->name, p->type1, p->type2,
        p->hp, p->attack, p->defense,
        p->sp_attack, p->sp_defense, p->speed,
        p->ability1, p->ability2, desc_escaped,
        prog && prog->encountered ? "true" : "false",
        prog && prog->caught ? "true" : "false"
    );
}

/**
 * Convert user progress summary to JSON
 */
void progress_to_json(UserProgress* progress, char* buffer, size_t size) {
    snprintf(buffer, size,
        "{"
        "\"total_seen\":%d,"
        "\"total_caught\":%d,"
        "\"total\":151"
        "}",
        progress->total_encountered,
        progress->total_caught
    );
}

/**
 * Convert Pokemon list to JSON array
 */
void list_to_json(PokedexData* pokedex, UserProgress* progress, 
                  bool caught_only, bool seen_only, char* buffer, size_t size) {
    char* ptr = buffer;
    size_t remaining = size;
    int written;
    
    written = snprintf(ptr, remaining, "[");
    ptr += written;
    remaining -= written;
    
    bool first = true;
    for (int i = 0; i < pokedex->count && remaining > 100; i++) {
        Pokemon* p = &pokedex->pokemon_array[i];
        ProgressEntry* prog = get_progress(progress, p->id);
        
        bool show = true;
        if (caught_only && (!prog || !prog->caught)) show = false;
        if (seen_only && (!prog || !prog->encountered || prog->caught)) show = false;
        
        if (show) {
            if (!first) {
                written = snprintf(ptr, remaining, ",");
                ptr += written;
                remaining -= written;
            }
            
            char poke_json[1024];
            pokemon_to_json(p, prog, poke_json, sizeof(poke_json));
            written = snprintf(ptr, remaining, "%s", poke_json);
            ptr += written;
            remaining -= written;
            
            first = false;
        }
    }
    
    snprintf(ptr, remaining, "]");
}
