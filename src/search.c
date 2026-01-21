/**
 * search.c - Search functionality
 * Implements BST operations and search functions
 */

#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "../include/pokemon.h"

/**
 * Search for a Pokemon by ID (O(1) lookup)
 */
Pokemon* search_by_id(PokedexData* pokedex, int id) {
    if (id < 1 || id > 151) {
        return NULL;
    }
    return &pokedex->pokemon_array[id - 1];
}

/**
 * Internal BST search helper
 */
static BSTNode* bst_search(BSTNode* root, const char* name) {
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

/**
 * Search for a Pokemon by name using BST (O(log n) average)
 */
Pokemon* search_by_name(PokedexData* pokedex, const char* name) {
    BSTNode* node = bst_search(pokedex->name_bst_root, name);
    return (node != NULL) ? node->pokemon : NULL;
}

/**
 * Insert a Pokemon into the BST (sorted by name)
 */
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

/**
 * Destroy BST and free all nodes
 */
void bst_destroy(BSTNode* root) {
    if (root == NULL) return;
    bst_destroy(root->left);
    bst_destroy(root->right);
    free(root);
}
