#ifndef TP2_ALGO_ALGOGRAM_H
#define TP2_ALGO_ALGOGRAM_H
#include <stdio.h>

typedef struct algogram algogram_t;
typedef struct post post_t;

// POST
post_t *crear_post(char* publicador, char* texto);

// ALGOGRAM

algogram_t *crear_algo(FILE *usuarios);
void login(algogram_t *algo, char* usuario);
void logout(algogram_t *algo);
void publicar(algogram_t *algo, char* texto);
void proximo_post(algogram_t *algo);
void likear_post(algogram_t *algo, size_t id);
void ver_likes(algogram_t *algo);


#endif //TP2_ALGO_ALGOGRAM_H
