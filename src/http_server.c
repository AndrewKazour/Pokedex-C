/**
 * http_server.c - HTTP server functionality
 * Handles HTTP requests and responses
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/pokemon.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#endif

#define BUFFER_SIZE 65536

/**
 * Send HTTP response to client
 */
void send_response(int client_sock, int status_code, const char* content_type, 
                   const char* body, size_t body_len) {
    char header[512];
    snprintf(header, sizeof(header),
        "HTTP/1.1 %d OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
        "Access-Control-Allow-Headers: Content-Type\r\n"
        "\r\n",
        status_code, content_type, body_len
    );
    
    send(client_sock, header, (int)strlen(header), 0);
    if (body && body_len > 0) {
        send(client_sock, body, (int)body_len, 0);
    }
}

/**
 * Handle incoming HTTP request
 */
void handle_request(int client_sock, const char* request,
                    PokedexData* pokedex, UserProgress* progress) {
    char method[16], path[256];
    sscanf(request, "%s %s", method, path);
    
    printf("Request: %s %s\n", method, path);
    
    // Handle OPTIONS for CORS
    if (strcmp(method, "OPTIONS") == 0) {
        send_response(client_sock, 200, "text/plain", "", 0);
        return;
    }
    
    char response[BUFFER_SIZE];
    
    // GET /api/progress
    if (strcmp(path, "/api/progress") == 0) {
        progress_to_json(progress, response, sizeof(response));
        send_response(client_sock, 200, "application/json", response, strlen(response));
    }
    // GET /api/list?filter=all|caught|seen
    else if (strncmp(path, "/api/list", 9) == 0) {
        bool caught_only = strstr(path, "caught") != NULL;
        bool seen_only = strstr(path, "seen") != NULL;
        
        list_to_json(pokedex, progress, caught_only, seen_only, 
                     response, sizeof(response));
        send_response(client_sock, 200, "application/json", response, strlen(response));
    }
    // GET /api/search?q=name or /api/search?id=25
    else if (strncmp(path, "/api/search", 11) == 0) {
        Pokemon* p = NULL;
        
        char* query = strstr(path, "q=");
        char* id_param = strstr(path, "id=");
        
        if (id_param) {
            int id = atoi(id_param + 3);
            p = search_by_id(pokedex, id);
        } else if (query) {
            char name[30];
            sscanf(query + 2, "%29[^&]", name);
            // URL decode (replace %20 with space)
            for (int i = 0; name[i]; i++) {
                if (name[i] == '+') name[i] = ' ';
            }
            p = search_by_name(pokedex, name);
        }
        
        if (p) {
            ProgressEntry* prog = get_progress(progress, p->id);
            pokemon_to_json(p, prog, response, sizeof(response));
            send_response(client_sock, 200, "application/json", response, strlen(response));
        } else {
            send_response(client_sock, 404, "application/json", 
                         "{\"error\":\"Pokemon not found\"}", 28);
        }
    }
    // POST /api/encounter?id=25
    else if (strncmp(path, "/api/encounter", 14) == 0) {
        char* id_param = strstr(path, "id=");
        if (id_param) {
            int id = atoi(id_param + 3);
            mark_encountered(progress, id);
            save_user_progress("user_progress.dat", progress);
            send_response(client_sock, 200, "application/json", "{\"success\":true}", 16);
        } else {
            send_response(client_sock, 400, "application/json", 
                         "{\"error\":\"Missing id\"}", 21);
        }
    }
    // POST /api/catch?id=25
    else if (strncmp(path, "/api/catch", 10) == 0) {
        char* id_param = strstr(path, "id=");
        if (id_param) {
            int id = atoi(id_param + 3);
            mark_caught(progress, id);
            save_user_progress("user_progress.dat", progress);
            send_response(client_sock, 200, "application/json", "{\"success\":true}", 16);
        } else {
            send_response(client_sock, 400, "application/json", 
                         "{\"error\":\"Missing id\"}", 21);
        }
    }
    // POST /api/reset?id=25 - Reset a specific Pokemon
    else if (strncmp(path, "/api/reset", 10) == 0) {
        char* id_param = strstr(path, "id=");
        if (id_param) {
            int id = atoi(id_param + 3);
            reset_pokemon(progress, id);
            save_user_progress("user_progress.dat", progress);
            send_response(client_sock, 200, "application/json", "{\"success\":true}", 16);
        } else {
            send_response(client_sock, 400, "application/json", 
                         "{\"error\":\"Missing id\"}", 21);
        }
    }
    // POST /api/reset-all - Reset all progress
    else if (strcmp(path, "/api/reset-all") == 0) {
        reset_all_progress(progress);
        save_user_progress("user_progress.dat", progress);
        send_response(client_sock, 200, "application/json", "{\"success\":true}", 16);
    }
    // Serve HTML
    else if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0) {
        FILE* f = fopen("pokedex.html", "r");
        if (f) {
            fseek(f, 0, SEEK_END);
            long fsize = ftell(f);
            fseek(f, 0, SEEK_SET);
            
            char* html = malloc(fsize + 1);
            fread(html, 1, fsize, f);
            html[fsize] = '\0';
            fclose(f);
            
            send_response(client_sock, 200, "text/html", html, fsize);
            free(html);
        } else {
            send_response(client_sock, 404, "text/plain", "404 Not Found", 13);
        }
    }
    else {
        send_response(client_sock, 404, "text/plain", "404 Not Found", 13);
    }
}
