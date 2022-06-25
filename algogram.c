#define  _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include "algogram.h"
#include "hash.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "cola.h"

// POST

struct post{
    char* publicador;
    char* texto;
    int cantidad_likes;
    hash_t *dieron_likes;
};

post_t *crear_post(char *publicador, char *texto) {
    post_t *post = malloc(sizeof (post_t));
    if (!post) return NULL;

    post->cantidad_likes = 0;
    post->dieron_likes = hash_crear(NULL);
    post->publicador = strdup(publicador);
    post->texto = strdup(texto);

    return post;
}

void destruir_post(post_t *post){
    hash_destruir(post->dieron_likes);
    free(post->publicador);
    free(post->texto);
    free(post);
}

void destruir_post_wrapper(void* post){
    destruir_post(post);
}

//-------------------------------------------------------------//

// USUARIO

struct usuario{
    int id;
    hash_t *feed;
};

void destruir_cola_wrapper(void* cola){
    cola_destruir(cola, free);
}

usuario_t *crear_usuario(int id) {
    usuario_t *usuario = malloc(sizeof (usuario_t));

    usuario->feed = hash_crear(destruir_cola_wrapper);
    usuario->id = id;
    return usuario;
}

void usuario_agregar_post(usuario_t *usuario, int id_post, int id_posteador) {
    if (id_posteador > usuario->id){
        id_posteador = id_posteador - usuario->id;
    } else if (id_posteador < usuario->id){
        id_posteador = usuario->id - id_posteador;
    } else{
        // Es el mismo usuario
        return;
    }

    char *id_posteador_puntero = malloc(sizeof (char*));
    sprintf(id_posteador_puntero, "%d", id_posteador);


    // Creo la cola si no existe
    if (!hash_pertenece(usuario->feed, id_posteador_puntero)){
        cola_t *cola = cola_crear();
        hash_guardar(usuario->feed, id_posteador_puntero, cola);
    }

    char *id_post_puntero = malloc(sizeof (char*));
    sprintf(id_post_puntero, "%d", id_post);

    cola_t *cola = hash_obtener(usuario->feed, id_posteador_puntero);
    cola_encolar(cola, id_post_puntero);
    free(id_posteador_puntero);

}

void destruir_usuario(usuario_t *usuario) {
    hash_destruir(usuario->feed);
    free(usuario);
}

void destruir_usuario_wrapper(void* usuario) {
    destruir_usuario(usuario);
}


//-----------------------------------------------------------------//

// ALGOGRAM

struct algogram{
    hash_t *usuarios_por_nombre; // para verificar si existen y obtener id por nombre
    hash_t *usuarios_por_id; // para guardar el feed, posts realizados
    hash_t *posts;
    int cant_posts;
    int cant_usuarios;
    int id_usuario_logueado;
    char* nombre_usuario_logueado;
    bool logueado;
};

algogram_t *crear_algo(FILE *usuarios){
    algogram_t *algo = malloc(sizeof (algogram_t));
    if (algo == NULL) return NULL;

    algo->cant_posts = 0;
    algo->nombre_usuario_logueado = NULL;
    algo->logueado = false;

    algo->posts = hash_crear(destruir_post_wrapper);
    if (!algo->posts){
        free(algo);
        return NULL;
    }

    algo->usuarios_por_nombre = hash_crear(free);
    if (!algo->usuarios_por_nombre){
        hash_destruir(algo->posts);
        free(algo);
        return NULL;
    }

    algo->usuarios_por_id = hash_crear(destruir_usuario_wrapper);
    if (!algo->usuarios_por_id){
        hash_destruir(algo->posts);
        hash_destruir(algo->usuarios_por_nombre);
        free(algo);
        return NULL;
    }

    char *line = NULL;
    size_t buffer_size = 0;
    int n = 0;
    while (getline(&line, &buffer_size, usuarios) != -1) {
        line[strcspn(line, "\n")] = 0;

        int* id = malloc(sizeof (int));
        *id = n;

        hash_guardar(algo->usuarios_por_nombre, line, (void*)id);

        char* id2 = malloc(sizeof (char*) + 13); // +13 para que no se queje el compilador
        sprintf(id2, "%d", n);

        usuario_t *usuario = crear_usuario(n);
        hash_guardar(algo->usuarios_por_id, id2, usuario);
        free(id2);
        n++;
    }
    free(line);

    algo->cant_usuarios = n;
    return algo;
}

void login(algogram_t *algo, char *usuario) {
    if (algo->logueado){
        printf("Error: Ya habia un usuario loggeado\n");
        return;
    } else if (!hash_pertenece(algo->usuarios_por_nombre, usuario)){
        printf("Error: usuario no existente\n");
        return;
    }

    if (algo->nombre_usuario_logueado)
        free(algo->nombre_usuario_logueado);

    printf("Hola %s\n", usuario);
    algo->logueado = true;
    algo->nombre_usuario_logueado = strdup(usuario);

    int id_usuario = *(int*)hash_obtener(algo->usuarios_por_nombre, usuario);
    algo->id_usuario_logueado = id_usuario;
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

    // Agrego el post al hash posts

    char* id_post = malloc(sizeof (char*));
    sprintf(id_post, "%d", algo->cant_posts);
    post_t *post = crear_post(algo->nombre_usuario_logueado, texto);
    hash_guardar(algo->posts, id_post, post);

    hash_iter_t *iter = hash_iter_crear(algo->usuarios_por_nombre);

    while (!hash_iter_al_final(iter)){
        const char* nombre_usuario = hash_iter_ver_actual(iter);

        int id_usuario = *(int*)hash_obtener(algo->usuarios_por_nombre, nombre_usuario);
        char* id_ = malloc(sizeof (char*));
        sprintf(id_, "%d", id_usuario);

        usuario_t *usuario = hash_obtener(algo->usuarios_por_id, id_);
        usuario_agregar_post(usuario, algo->cant_posts, algo->id_usuario_logueado);
        hash_iter_avanzar(iter);
        free(id_);
    }

    free(id_post);
    hash_iter_destruir(iter);
    algo->cant_posts++;
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

    if (hash_obtener(post->dieron_likes, algo->nombre_usuario_logueado) == NULL){
        hash_guardar(post->dieron_likes, algo->nombre_usuario_logueado, (void*)true);
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
    free(algo->nombre_usuario_logueado);
    hash_destruir(algo->posts);
    hash_destruir(algo->usuarios_por_nombre);
    hash_destruir(algo->usuarios_por_id);
    free(algo);
}



void ver_siguiente_feed(algogram_t *algo){
    if (!algo->logueado){
        printf("Usuario no loggeado o no hay mas posts para ver\n");
        return;
    }

    char* id_usuario = malloc(sizeof (char*));
    sprintf(id_usuario, "%d", algo->id_usuario_logueado);

    usuario_t *usuario = hash_obtener(algo->usuarios_por_id, id_usuario);

    char* id_post = NULL;
    for (int i = 0; i < algo->cant_usuarios; i++){
        char * i_puntero = malloc(sizeof (char*));
        sprintf(i_puntero, "%d", i);
        if (!hash_pertenece(usuario->feed, i_puntero)) {
            free(i_puntero);
            continue;
        }
        cola_t *cola = hash_obtener(usuario->feed, i_puntero);
        free(i_puntero);
        if (!cola_esta_vacia(cola)){
            id_post = cola_desencolar(cola);
            break;
        }
    }

    free(id_usuario);

    if(id_post == NULL){
        printf("Usuario no loggeado o no hay mas posts para ver\n");
        return;
    }

    post_t *post = hash_obtener(algo->posts, id_post);

    printf("Post ID %s\n", id_post);
    printf("%s dijo: %s\n", post->publicador, post->texto);
    printf("Likes: %i\n", post->cantidad_likes);
}

