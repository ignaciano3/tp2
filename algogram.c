//
// Created by ignacio on 6/6/22.
//

#include "algogram.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct post{
    //int id; //no se si se necesita un id, ya la posicion es el id
    char* publicador;
    char* texto;
    int cantidad_likes;
    char** dieron_likes;
};

post_t *crear_post(char *publicador, char *texto) {
    post_t *post = malloc(sizeof (post_t));
    if (!post) return NULL;

    post->cantidad_likes = 0;
    post->dieron_likes = malloc(sizeof (char*) * 10);
    post->publicador = strdup(publicador);
    post->texto = strdup(texto);

    return post;
}

struct algogram{
    char** usuarios; //va a ser un hash
    post_t **posts;
    int cant_posts;
    bool logueado;
    char* usuario_logueado;
};

algogram_t *crear_algo(FILE *usuarios){
    algogram_t *algo = malloc(sizeof (algogram_t));
    if (algo == NULL) return NULL;
    algo->cant_posts = 0;
    algo->posts = malloc(sizeof (post_t) * 10);
    algo->usuario_logueado = malloc(sizeof (char*));
    if (!algo->posts){
        free(algo);
        return NULL;
    }
    if (!algo->usuario_logueado){
        free(algo);
        return NULL;
    }
    return algo;
}

void login(algogram_t *algo, char *usuario) {
    if (algo->logueado){
        printf("Error: Ya habia un usuario loggeado\n");
        return;
    }
    /*
    else if (usuario no existe){
        printf("Error: usuario no existente");
    }
    */
    printf("Hola %s\n", usuario);
    algo->logueado = true;
    algo->usuario_logueado = strdup(usuario);
}

void logout(algogram_t *algo) {
    if (!algo->logueado){
        printf("Error: no habia usuario loggeado\n");
        return;
    }
    printf("Adios\n");
    algo->logueado = false;
}

void publicar(algogram_t *algo, char *texto) {
    if (!algo->logueado){
        printf("Error: no habia usuario loggeado\n");
        return;
    }

    post_t *post = crear_post(algo->usuario_logueado, texto);
    algo->posts[algo->cant_posts] = post;
    algo->cant_posts++;
    printf("Post publicado\n");

}

void likear_post(algogram_t *algo, size_t id) {
    if (!algo->logueado || algo->cant_posts <= id){
        printf("Error: Usuario no loggeado o Post inexistente\n");
        return;
    }
    //hay que checkear si ya dio like
    algo->posts[id]->dieron_likes[algo->posts[id]->cantidad_likes] = algo->usuario_logueado;
    algo->posts[id]->cantidad_likes++;
    printf("Post likeado\n");

}
