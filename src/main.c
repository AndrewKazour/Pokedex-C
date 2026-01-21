/**
 * main.c - Entry point for Pokedex server
 * Initializes data and starts the HTTP server
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/pokemon.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    typedef int socklen_t;
    #define closesocket closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define closesocket close
#endif

#define PORT 8080
#define BUFFER_SIZE 65536

// Global data
static PokedexData global_pokedex;
static UserProgress global_progress;

int main(int argc, char* argv[]) {
    #ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
    #endif
    
    printf("=== Pokedex Server Starting ===\n");
    fflush(stdout);
    
    // Check for command line arguments
    bool reset_progress = false;
    bool reset_pokemon_id = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--reset") == 0 || strcmp(argv[i], "-r") == 0) {
            reset_progress = true;
        }
        else if (strcmp(argv[i], "--reset-id") == 0 && i + 1 < argc) {
            reset_pokemon_id = atoi(argv[++i]);
        }
        else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Usage: pokedex_server [options]\n");
            printf("Options:\n");
            printf("  --reset, -r       Reset all progress on startup\n");
            printf("  --reset-id <id>   Reset specific Pokemon by ID\n");
            printf("  --help, -h        Show this help message\n");
            return 0;
        }
    }
    
    printf("Loading Pokemon data...\n");
    fflush(stdout);
    
    if (!load_pokemon_data("pokemon_data.csv", &global_pokedex)) {
        printf("Failed to load Pokemon data!\n");
        fflush(stdout);
        return 1;
    }
    
    load_user_progress("user_progress.dat", &global_progress);
    
    // Handle reset options
    if (reset_progress) {
        printf("Resetting ALL progress...\n");
        reset_all_progress(&global_progress);
        save_user_progress("user_progress.dat", &global_progress);
        printf("Progress reset complete!\n");
    }
    else if (reset_pokemon_id > 0) {
        printf("Resetting Pokemon #%d...\n", reset_pokemon_id);
        reset_pokemon(&global_progress, reset_pokemon_id);
        save_user_progress("user_progress.dat", &global_progress);
        printf("Pokemon #%d reset complete!\n", reset_pokemon_id);
    }
    
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        printf("Socket creation failed\n");
        return 1;
    }
    
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Bind failed\n");
        return 1;
    }
    
    listen(server_sock, 5);
    
    printf("Server running on http://localhost:%d\n", PORT);
    printf("Open your browser and go to http://localhost:%d\n\n", PORT);
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
        
        if (client_sock < 0) continue;
        
        char buffer[BUFFER_SIZE];
        int received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
        if (received > 0) {
            buffer[received] = '\0';
            handle_request(client_sock, buffer, &global_pokedex, &global_progress);
        }
        
        closesocket(client_sock);
    }
    
    closesocket(server_sock);
    bst_destroy(global_pokedex.name_bst_root);
    
    #ifdef _WIN32
    WSACleanup();
    #endif
    
    return 0;
}
