#define  _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include <stdio.h>

#ifndef TP2_ALGO_ALGOGRAM_H
#define TP2_ALGO_ALGOGRAM_H

typedef struct algogram algogram_t;
typedef struct post post_t;
typedef struct usuario usuario_t;

// USUARIO
usuario_t *crear_usuario(int id);
void usuario_agregar_post(usuario_t *usuario, int id_post, int id_posteador);
void destruir_usuario(usuario_t *usuario);
void destruir_usuario_wrapper(void *usuario);
int cmp_feed(const void*a, const void*b);

// POST
post_t *crear_post(char* publicador, char* texto);
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
void ver_siguiente_feed(algogram_t *algo);


#endif //TP2_ALGO_ALGOGRAM_H
