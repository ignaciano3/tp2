#define  _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <stdio.h>
#include "heap.h"
#include "lista.h"

#ifndef TP2_ALGO_ALGOGRAM_H
#define TP2_ALGO_ALGOGRAM_H

typedef struct algogram algogram_t;
typedef struct post post_t;
typedef struct usuario usuario_t;

// USUARIO
usuario_t *crear_usuario();
void usuario_agregar_post(usuario_t *usuario, int id_post);
void destruir_usuario(usuario_t *usuario);
void destruir_usuario_wrapper(void *usuario);
lista_t *ver_posts_usuario(usuario_t* usuario);

// POST
post_t *crear_post(char* publicador, char* texto, int id);
void destruir_post(post_t *post);
void destruir_post_wrapper(void *post);

// ALGOGRAM
algogram_t *crear_algo(FILE *usuarios);
bool login(algogram_t *algo, char* usuario);
bool logout(algogram_t *algo);
void publicar(algogram_t *algo, char* texto);
void likear_post(algogram_t *algo, size_t id);
void mostrar_likes(algogram_t *algo, size_t id);
void destruir_algo(algogram_t *algo);
int ver_siguiente_feed(algogram_t *algo, heap_t *feed, int id_visitar);

// FEED
bool actualizar_feed(algogram_t *algo, heap_t* feed, int ult_dist);

#endif //TP2_ALGO_ALGOGRAM_H
