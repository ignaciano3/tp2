#define  _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include "algogram.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define BASE 10
#include "heap.h"

int cmp(const void* a, const void *b){
    return *(int*)b - *(int*)a;
}

int main(int argc, char **argv){
    if (argc == 1) return 1;
    FILE *usuarios = fopen(argv[1], "r");
    algogram_t *algo = crear_algo(usuarios);

    char *line = NULL;
    size_t buffer_size = 0;
    ssize_t gl;

    heap_t* feed = heap_crear(cmp);
    int ult_dist = 1;

    while (1) {
        gl = getline(&line, &buffer_size, stdin);
        if (gl == -1) break;
        line[strcspn(line, "\n")] = 0; // remuevo el \n

        if (strcmp(line, "login") == 0){
            gl = getline(&line, &buffer_size, stdin);
            if (gl == -1) break;
            line[strcspn(line, "\n")] = 0;
            bool loggeo = login(algo, line);
            if (!loggeo) continue;
            ult_dist = 1; // este ult dist tiene que ser particular de cada usuario
        }

        else if (strcmp(line, "logout") == 0){
            bool desloggeo = logout(algo);
            if (!desloggeo) continue;
            heap_destruir(feed, NULL);
            feed = heap_crear(cmp);
        }

        else if (strcmp(line, "publicar") == 0){
            gl = getline(&line, &buffer_size, stdin);
            if (gl == -1) break;
            line[strcspn(line, "\n")] = 0;
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
            ult_dist = ver_siguiente_feed(algo, feed, ult_dist);
        }

        // exit es ctrl + d
    }
    heap_destruir(feed, NULL);
    free(line);
    destruir_algo(algo);
    fclose(usuarios);
    return 0;
}