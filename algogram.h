#define  _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <stdio.h>
#include "heap.h"

#ifndef TP2_ALGO_ALGOGRAM_H
#define TP2_ALGO_ALGOGRAM_H

typedef struct algogram algogram_t;
typedef struct post post_t;

// POST
post_t *crear_post(char* publicador, char* texto, int id);
void destruir_post(post_t *post);
void destruir_post_wrapper(void *post);

// ALGOGRAM
algogram_t *crear_algo(FILE *usuarios);
void login(algogram_t *algo, char* usuario);
void logout(algogram_t *algo);
void publicar(algogram_t *algo, char* texto);
void likear_post(algogram_t *algo, size_t id);
void mostrar_likes(algogram_t *algo, size_t id);
void destruir_algo(algogram_t *algo);

#endif //TP2_ALGO_ALGOGRAM_H