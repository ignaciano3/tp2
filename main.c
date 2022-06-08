#include "algogram.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define BASE 10

int main (int argc, char **argv){
    // se supone que se introducen usuarios y son validos
    FILE *usuarios = fopen(argv[1], "r");
    algogram_t *algo = crear_algo(usuarios);

    char *line = NULL;
    size_t buffer_size = 0;

    while (getline(&line, &buffer_size, stdin) != -1) {
        line[strcspn(line, "\n")] = 0; // remuevo el \n

        if (strcmp(line, "login") == 0){
            getline(&line, &buffer_size, stdin);
            line[strcspn(line, "\n")] = 0;
            login(algo, line);
        }

        if (strcmp(line, "logout") == 0){
            logout(algo);
        }

        if (strcmp(line, "publicar") == 0){
            getline(&line, &buffer_size, stdin);
            line[strcspn(line, "\n")] = 0;
            publicar(algo, line);
        }

        if (strcmp(line, "likear_post") == 0){
            getline(&line, &buffer_size, stdin);
            line[strcspn(line, "\n")] = 0;
            size_t n = (size_t)strtol(line, NULL, BASE);
            likear_post(algo, n);
        }

        if (strcmp(line, "mostrar_likes") == 0){
            getline(&line, &buffer_size, stdin);
            line[strcspn(line, "\n")] = 0;
            size_t n = (size_t)strtol(line, NULL, BASE);
            mostrar_likes(algo, n);
        }

        if (strcmp(line, "ver_siguiente_feed") == 0){
            ver_siguiente_feed(algo);
        }

        // exit es ctrl + d
    }
    free(line);
    destruir_algo(algo);
    fclose(usuarios);
    return 0;
}