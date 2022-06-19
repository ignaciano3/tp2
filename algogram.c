#define  _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include "algogram.h"
#include "hash.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// POST

struct post{
    int id;
    char* publicador;
    char* texto;
    int cantidad_likes;
    hash_t *dieron_likes;
};

post_t *crear_post(char *publicador, char *texto, int id) {
    post_t *post = malloc(sizeof (post_t));
    if (!post) return NULL;

    post->cantidad_likes = 0;
    post->dieron_likes = hash_crear(NULL);
    post->publicador = strdup(publicador);
    post->texto = strdup(texto);
    post->id = id;

    return post;
}

void destruir_post_wrapper(void* post){
    destruir_post(post);
}

void destruir_post(post_t *post){
    hash_destruir(post->dieron_likes);
    free(post->publicador);
    free(post->texto);
    free(post);
}

//-------------------------------------------------------------//

// ALGOGRAM

struct algogram{
    hash_t *usuarios;
    hash_t *posts;
    int cant_posts;
    bool logueado;
    char* usuario_logueado;
};

algogram_t *crear_algo(FILE *usuarios){
    algogram_t *algo = malloc(sizeof (algogram_t));
    if (algo == NULL) return NULL;

    algo->cant_posts = 0;
    algo->usuario_logueado = NULL;
    algo->logueado = false;

    algo->posts = hash_crear(destruir_post_wrapper);
    if (!algo->posts){
        free(algo);
        return NULL;
    }

    algo->usuarios = hash_crear(NULL);
    if (!algo->usuarios){
        hash_destruir(algo->posts);
        free(algo);
        return NULL;
    }
    char *line = NULL;
    size_t buffer_size = 0;

    while (getline(&line, &buffer_size, usuarios) != -1) {
        line[strcspn(line, "\n")] = 0;
        hash_guardar(algo->usuarios, line, NULL);
    }

    free(line);
    return algo;
}

void login(algogram_t *algo, char *usuario) {
    if (algo->logueado){
        printf("Error: Ya habia un usuario loggeado\n");
        return;
    } else if (!hash_pertenece(algo->usuarios, usuario)){
        printf("Error: usuario no existente\n");
        return;
    }
    if (algo->usuario_logueado){
        free(algo->usuario_logueado);
    }

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
    char* id = malloc(sizeof (char*));
    sprintf(id, "%d", algo->cant_posts);
    post_t *post = crear_post(algo->usuario_logueado, texto, algo->cant_posts);
    hash_guardar(algo->posts, id, post);
    algo->cant_posts++;
    free(id);
    printf("Post publicado\n");

}

void likear_post(algogram_t *algo, size_t id) {
    if (!algo->logueado || algo->cant_posts <= id){
        printf("Error: Usuario no loggeado o Post inexistente\n");
        return;
    }

    char* id_ = malloc(sizeof (char*));
    sprintf(id_, "%d", (int)id);
    post_t *post = hash_obtener(algo->posts, id_);

    if (hash_obtener(post->dieron_likes, algo->usuario_logueado) == NULL){
        hash_guardar(post->dieron_likes, algo->usuario_logueado, (void*)true);
        post->cantidad_likes++;
    }
    free(id_);
    printf("Post likeado\n");

}

void mostrar_likes(algogram_t *algo, size_t id) {
    char* id_ = malloc(sizeof (char*));
    sprintf(id_, "%d", (int)id);

    post_t *post = hash_obtener(algo->posts, id_);
    if (algo->cant_posts <= id || post == NULL){
        printf("Error: Post inexistente o sin likes\n");
    }

    int n = post->cantidad_likes;
    printf("El post tiene %i likes\n", n);
    hash_iter_t *iter = hash_iter_crear(post->dieron_likes);
    while(!hash_iter_al_final(iter)){
        printf("    %s\n", hash_iter_ver_actual(iter));
        hash_iter_avanzar(iter);
    }
    hash_iter_destruir(iter);
    free(id_);
}

void destruir_algo(algogram_t *algo) {
    free(algo->usuario_logueado);
    hash_destruir(algo->posts);
    hash_destruir(algo->usuarios);
    free(algo);
}
