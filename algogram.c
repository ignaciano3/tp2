#define  _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#include "algogram.h"
#include "hash.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "abb.h"
#include "heap.h"
// POST

int cmp_feed(const void*a, const void*b){
    // "1,28" USUARIO POS 1, ID POST 28
    char* resto_a;
    char* resto_b;
    size_t pos_usuario_a = (size_t)strtol((char*)a, &resto_a, 10);
    size_t pos_usuario_b = (size_t)strtol((char*)b, &resto_b, 10);

    if (pos_usuario_a > pos_usuario_b){
        return -1;
    } else if (pos_usuario_a < pos_usuario_b){
        return 1;
    }
    resto_a++; //para quitar la coma
    resto_b++;
    size_t id_post_a = (size_t)strtol(resto_a, NULL, 10);
    size_t id_post_b = (size_t)strtol(resto_b, NULL, 10);

    if (id_post_a > id_post_b){
        return -1;
    } else if (id_post_a < id_post_b){
        return 1;
    } else {
        return 0; //no deberia pasar hasta aca
    }
}

struct post{
    char* publicador;
    char* texto;
    abb_t* dieron_likes;
};

post_t *crear_post(char *publicador, char *texto) {
    post_t *post = malloc(sizeof (post_t));
    if (!post) return NULL;

    post->dieron_likes = abb_crear(strcmp, NULL);
    if (!post->dieron_likes){
        free(post);
        return NULL;
    }
    post->publicador = strdup(publicador);
    if (!post->publicador){
        free(post->dieron_likes);
        free(post);
        return NULL;
    }
    post->texto = strdup(texto);
    if (!post->texto){
        free(post->dieron_likes);
        free(post->publicador);
        free(post);
        return NULL;
    }

    return post;
}

void destruir_post(post_t *post){
    abb_destruir(post->dieron_likes);
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
    heap_t *feed;
};

usuario_t *crear_usuario(int id) {
    usuario_t *usuario = malloc(sizeof (usuario_t));
    if (!usuario) return NULL;
    usuario->feed = heap_crear(cmp_feed);
    if (!usuario->feed){
        free(usuario);
        return NULL;
    }
    usuario->id = id;
    return usuario;
}

void usuario_agregar_post(usuario_t *usuario, int id_post, int id_posteador) {
    if (id_posteador > usuario->id){
        id_posteador = id_posteador - usuario->id;
    } else if (id_posteador < usuario->id){
        id_posteador = usuario->id - id_posteador;
    } else{
        return; // Es el mismo usuario
    }

    char *id_posteador_puntero = malloc(sizeof (char*)+3);
    if (!id_posteador_puntero) return;

    sprintf(id_posteador_puntero, "%d", id_posteador);

    void *id_post_puntero = malloc(sizeof (void*));
    if (!id_post_puntero){
        free(id_posteador_puntero);
        return;
    }
    sprintf(id_post_puntero, "%d", id_post);

    strcat(id_posteador_puntero, ",");
    strcat(id_posteador_puntero, id_post_puntero);
    free(id_post_puntero);

    heap_encolar(usuario->feed, id_posteador_puntero);

}

void destruir_usuario(usuario_t *usuario) {
    heap_destruir(usuario->feed, free);
    free(usuario);
}

void destruir_usuario_wrapper(void* usuario) {
    destruir_usuario(usuario);
}


//-----------------------------------------------------------------//

// ALGOGRAM

struct algogram{
    hash_t *usuarios_por_nombre; // para verificar si existen y obtener id por nombre
    hash_t *usuarios_por_id; // para guardar el feed
    hash_t *posts;
    int cant_posts;
    int cant_usuarios;
    int id_usuario_logueado;
    char* nombre_usuario_logueado;
    bool logueado;
};

algogram_t *crear_algo(FILE *usuarios){
    algogram_t *algo = malloc(sizeof (algogram_t));
    if (!algo) return NULL;

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
        if (!id){
            break;
        }

        *id = n;
        char* nombre = strdup(line);
        if (!nombre){
            free(id);
            break;
        }

        hash_guardar(algo->usuarios_por_nombre, nombre, (void*)id);
        free(nombre);

        char* id2 = malloc(sizeof (char*) + 3); // +3 para que no se queje el compilador

        if (!id2){
            break;
        }
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

    char* id_post = malloc(sizeof (char*) +3); //no necesita +3
    if (!id_post){
        return;
    }

    sprintf(id_post, "%d", algo->cant_posts);
    post_t *post = crear_post(algo->nombre_usuario_logueado, texto);
    hash_guardar(algo->posts, id_post, post);

    hash_iter_t *iter = hash_iter_crear(algo->usuarios_por_nombre);

    while (!hash_iter_al_final(iter)){
        const char* nombre_usuario = hash_iter_ver_actual(iter);

        int id_usuario = *(int*)hash_obtener(algo->usuarios_por_nombre, nombre_usuario);
        char* id_ = malloc(sizeof (char*) +3);//no necesita +3
        if (!id_){
            break;
        }
        sprintf(id_, "%d", id_usuario);

        usuario_t *usuario = hash_obtener(algo->usuarios_por_id, id_);
        free(id_);
        usuario_agregar_post(usuario, algo->cant_posts, algo->id_usuario_logueado);
        hash_iter_avanzar(iter);

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
    printf("Post likeado\n");

    char* id_ = malloc(sizeof (char*) +3); //no necesita +3
    if (!id_) return;
    sprintf(id_, "%d", (int)id);
    post_t *post = hash_obtener(algo->posts, id_);
    free(id_);

    char* este_usuario = strdup(algo->nombre_usuario_logueado);
    if (!este_usuario) return;
    abb_guardar(post->dieron_likes, este_usuario, NULL);
    free(este_usuario);

}

bool printear_nombres(const char*nombre, void* dato, void* extra){
    printf("\t%s\n", nombre);
    return true;
}

void mostrar_likes(algogram_t *algo, size_t id) {
    char* id_ = malloc(sizeof (char*) +3); //no necesita +3
    if (!id_) return;

    sprintf(id_, "%d", (int)id);
    post_t *post = hash_obtener(algo->posts, id_);
    free(id_);

    if (algo->cant_posts <= id || post == NULL){
        printf("Error: Post inexistente o sin likes\n");
        return;
    }

    size_t n = abb_cantidad(post->dieron_likes);
    if (n == 0){
        printf("Error: Post inexistente o sin likes\n");
        return;
    }
    printf("El post tiene %zu likes:\n", n);

    abb_in_order(post->dieron_likes, printear_nombres, NULL);

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

    char* id_usuario = malloc(sizeof (char*) +3); //no necesita +3
    if (!id_usuario) return;
    sprintf(id_usuario, "%d", algo->id_usuario_logueado);
    usuario_t *usuario = hash_obtener(algo->usuarios_por_id, id_usuario);
    free(id_usuario);

    if(heap_esta_vacio(usuario->feed)){
        printf("Usuario no loggeado o no hay mas posts para ver\n");
        return;
    }

    char* clave_post = heap_desencolar(usuario->feed);
    char* resto;
    strtol(clave_post, &resto, 10);
    resto++;
    size_t post_id = (size_t)strtol(resto, NULL, 10);

    char* id_post = malloc(sizeof (char*)+3);//no necesita
    if (!id_post){
        free(clave_post);
        return;
    };
    sprintf(id_post, "%zu", post_id);

    post_t *post = hash_obtener(algo->posts, id_post);

    printf("Post ID %s\n", id_post);
    free(id_post);
    printf("%s dijo: %s", post->publicador, post->texto);
    printf("Likes: %zu\n", abb_cantidad(post->dieron_likes));
    free(clave_post);
}
