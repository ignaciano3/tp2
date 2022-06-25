#include "cola.h"
#include <stddef.h>
#include <malloc.h>

typedef struct nodo{
    void* valor;
    struct nodo* siguiente;
}nodo_t;

nodo_t *nodo_crear(void *valor) {
    nodo_t* nodo = malloc(sizeof (nodo_t));
    if (nodo == NULL)
        return NULL;
    nodo->valor = valor;
    nodo->siguiente = NULL;
    return nodo;
}

struct cola {
    nodo_t* primero;
    nodo_t* ultimo;
};

cola_t *cola_crear(void) {
    cola_t* cola = malloc(sizeof (cola_t));
    if (cola == NULL)
        return NULL;
    cola->primero=NULL;
    cola->ultimo=NULL;
    return cola;
}

bool cola_esta_vacia(const cola_t *cola) {
    return cola->primero == NULL;
}

void *cola_ver_primero(const cola_t *cola) {
    if (cola_esta_vacia(cola))
        return NULL;
    return cola->primero->valor;
}

bool cola_encolar(cola_t *cola, void *valor) {
    nodo_t* nuevo_nodo = nodo_crear(valor);
    if (nuevo_nodo == NULL)
        return false;
    if (cola_esta_vacia(cola)) {
        cola->primero = nuevo_nodo;
        cola->ultimo = nuevo_nodo;
    } else {
        cola->ultimo->siguiente = nuevo_nodo;
        cola->ultimo = nuevo_nodo;
    }
    return true;

}

void *cola_desencolar(cola_t *cola) {
    if (cola_esta_vacia(cola))
        return NULL;
    nodo_t* primero = cola->primero;
    void* valor = cola->primero->valor;
    if (cola->primero->siguiente == NULL){ //Si hay solo 1 elemento
        free(primero);
        cola->primero = NULL;
        cola->ultimo = NULL;
    } else {
        cola->primero = cola->primero->siguiente;
        free(primero);
    }
    return valor;
}

void cola_destruir(cola_t *cola, void (*destruir_dato)(void *)) {
    while (cola->primero != NULL){
        void* prim = cola_desencolar(cola);
        if (destruir_dato != NULL) {
            destruir_dato(prim);
        }
    }
    free(cola);
}