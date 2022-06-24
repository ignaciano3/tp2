#define  _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include "algogram.h"
#include "hash.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "lista.h"


// POST

struct post{
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
    hash_t *usuarios_por_nombre; //para verificar si existen
    hash_t *usuarios_por_id; //para guardar el feed
    hash_t *posts;
    int cant_posts;
    int cant_usuarios;
    bool logueado;
    char* nombre_usuario_logueado;
    int id_usuario_logueado;
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

        char* id2 = malloc(sizeof (char*) + 13);
        sprintf(id2, "%d", n);

        usuario_t *usuario = crear_usuario();
        hash_guardar(algo->usuarios_por_id, id2, usuario);
        free(id2);
        n++;
    }
    free(line);

    algo->cant_usuarios = n;
    return algo;
}

bool login(algogram_t *algo, char *usuario) {
    if (algo->logueado){
        printf("Error: Ya habia un usuario loggeado\n");
        return false;
    } else if (!hash_pertenece(algo->usuarios_por_nombre, usuario)){
        printf("Error: usuario no existente\n");
        return false;
    }
    if (algo->nombre_usuario_logueado){
        free(algo->nombre_usuario_logueado);
    }

    printf("Hola %s\n", usuario);
    algo->logueado = true;
    algo->nombre_usuario_logueado = strdup(usuario);

    int id_usuario = *(int*)hash_obtener(algo->usuarios_por_nombre, usuario);
    algo->id_usuario_logueado = id_usuario;
    return true;
}

bool logout(algogram_t *algo) {
    if (!algo->logueado){
        printf("Error: no habia usuario loggeado\n");
        return false;
    }
    printf("Adios\n");
    algo->logueado = false;
    return true;
}

void publicar(algogram_t *algo, char *texto) {
    if (!algo->logueado){
        printf("Error: no habia usuario loggeado\n");
        return;
    }

    // Agrego el post al hash posts

    char* id_post = malloc(sizeof (char*));
    sprintf(id_post, "%d", algo->cant_posts);
    post_t *post = crear_post(algo->nombre_usuario_logueado, texto, algo->cant_posts);
    hash_guardar(algo->posts, id_post, post);
    free(id_post);

    // Agrego el id del post al id del hash de usuario_t

    char* id_usuario = malloc(sizeof (char*));
    sprintf(id_usuario, "%d", algo->id_usuario_logueado);
    usuario_t *usuario = hash_obtener(algo->usuarios_por_id, id_usuario);
    usuario_agregar_post(usuario, algo->cant_posts);
    free(id_usuario);

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

bool actualizar_feed(algogram_t *algo, heap_t *feed, int id_visitar) {
    bool cambio = false;

    if (id_visitar >= 0 && id_visitar < algo->cant_usuarios){
        char* id_ = malloc(sizeof (char*));
        sprintf(id_, "%d", id_visitar);
        usuario_t *usuario = hash_obtener(algo->usuarios_por_id, id_);
        lista_iter_t* iter = lista_iter_crear(ver_posts_usuario(usuario));

        while (!lista_iter_al_final(iter)){
            heap_encolar(feed, lista_iter_ver_actual(iter));
            lista_iter_avanzar(iter);
        }
        lista_iter_destruir(iter);
        free(id_);
        cambio = true;
    }

    return cambio;
}

int ver_siguiente_feed(algogram_t *algo, heap_t*feed, int ult_dist){
    if (!algo->logueado){
        printf("Usuario no loggeado o no hay mas posts para ver\n");
        return ult_dist;
    }

    while (heap_esta_vacio(feed)){
        // mas que cambio es que hay usuarios para inspeccionar
        bool cambio = actualizar_feed(algo, feed, algo->id_usuario_logueado - ult_dist);
        cambio |= actualizar_feed(algo, feed, algo->id_usuario_logueado + ult_dist);
        ult_dist++;
        if (!cambio){
            printf("Usuario no loggeado o no hay mas posts para ver\n");
            return ult_dist;
        }
    }

    int id_post = *(int*)heap_desencolar(feed);
    char* id_ = malloc(sizeof (char*));
    sprintf(id_, "%d", (int)id_post);

    post_t *post = hash_obtener(algo->posts, id_);

    printf("Post ID %i\n", id_post);
    printf("%s dijo: %s\n", post->publicador, post->texto);
    printf("Likes: %i\n", post->cantidad_likes);
    free(id_);
    return ult_dist;
}


// USUARIO


struct usuario{
    lista_t * posts_realizados;
};

usuario_t *crear_usuario() {
    usuario_t *usuario = malloc(sizeof (usuario_t));
    usuario->posts_realizados = lista_crear();
    return usuario;
}

void usuario_agregar_post(usuario_t *usuario, int id_post) {
    int* id = malloc(sizeof (int));
    *id = id_post;
    lista_insertar_ultimo(usuario->posts_realizados, (void*)id);
}

void destruir_usuario(usuario_t *usuario) {
    lista_destruir(usuario->posts_realizados, free);
    free(usuario);
}

void destruir_usuario_wrapper(void* usuario) {
    destruir_usuario(usuario);
}

lista_t *ver_posts_usuario(usuario_t *usuario) {
    return usuario->posts_realizados;
}


