#include "algogram.h"
#include "hash.h"
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

void destruir_post(post_t *post){
    free(post->dieron_likes);
    free(post->publicador);
    free(post->texto);
    free(post);
}

struct algogram{
    hash_t* usuarios;
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
    algo->usuarios = hash_crear(NULL);

    if (!algo->posts || !algo->usuario_logueado || !algo->usuarios){
        free(algo);
        return NULL;
    }
    char *line = NULL;
    size_t buffer_size = 0;
    ssize_t characters = 0;

    int ubicacion = 0;
    while (getline(&line, &buffer_size, usuarios) != -1) {
        line[strcspn(line, "\n")] = 0;
        hash_guardar(algo->usuarios, line, NULL); //aca pienso en guardar su ubicacion

    }
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

void mostrar_likes(algogram_t *algo, size_t id) {
    if (algo->cant_posts <= id || algo->posts[id]->cantidad_likes == 0){
        printf("Error: Post inexistente o sin likes\n");
    }

    int n = algo->posts[id]->cantidad_likes;
    printf("El post tiene %i likes\n", n);
    for (int i = 0; i < n; i++){
        printf("    %s\n", algo->posts[id]->dieron_likes[i]);
    }
}

void ver_siguiente_feed(algogram_t *algo) {

}

void destruir_algo(algogram_t *algo) {
    free(algo->usuario_logueado);
    for (int i = 0; i < algo->cant_posts; i++){
        destruir_post(algo->posts[i]);
    }
    free(algo->posts);
    hash_destruir(algo->usuarios);
    free(algo);

}
