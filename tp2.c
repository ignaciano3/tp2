#define  _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include "algogram.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BASE 10

int main(int argc, char **argv){
    if (argc == 1) return 1;
    FILE *usuarios = fopen(argv[1], "r");

    algogram_t *algo = crear_algo(usuarios);
    fclose(usuarios);

    char *line = NULL;
    size_t buffer_size = 0;
    ssize_t gl;

    while (1) {
        gl = getline(&line, &buffer_size, stdin);
        if (gl == -1) break;
        line[strcspn(line, "\n")] = 0; // remuevo el \n

        if (strcmp(line, "login") == 0){
            gl = getline(&line, &buffer_size, stdin);
            if (gl == -1) break;
            line[strcspn(line, "\n")] = 0;
            login(algo, line);
        }

        else if (strcmp(line, "logout") == 0){
            logout(algo);
        }

        else if (strcmp(line, "publicar") == 0){
            gl = getline(&line, &buffer_size, stdin);
            if (gl == -1) break;
            publicar(algo, line);
        }

        else if (strcmp(line, "likear_post") == 0){
            gl = getline(&line, &buffer_size, stdin);
            if (gl == -1) break;
            line[strcspn(line, "\n")] = 0;
            size_t n = (size_t)strtol(line, NULL, BASE);
            likear_post(algo, n);
        }

        else if (strcmp(line, "mostrar_likes") == 0){
            gl = getline(&line, &buffer_size, stdin);
            if (gl == -1) break;
            line[strcspn(line, "\n")] = 0;
            size_t n = (size_t)strtol(line, NULL, BASE);
            mostrar_likes(algo, n);
        }

        else if (strcmp(line, "ver_siguiente_feed") == 0){
           ver_siguiente_feed(algo);
        }

        // exit es ctrl + d
    }
    free(line);
    destruir_algo(algo);
    return 0;
}