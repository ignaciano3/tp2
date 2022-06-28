#define  _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

#include "abb.h"
#include "pila.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct nodo nodo_t;

struct nodo {
    nodo_t *izq, *der;
    void *dato;
    char *clave;
};

nodo_t *crear_nodo(const char *clave, void *dato){
    nodo_t *nodo = malloc(sizeof (nodo_t));
    if (nodo == NULL) return NULL;
    nodo->clave = strdup(clave);
    nodo->dato = dato;
    nodo->izq = nodo->der = NULL;
    return nodo;
}

void* destruir_nodo(nodo_t* nodo){
    void* dato = nodo->dato;
    free(nodo->clave);
    free(nodo);
    return dato;
}

//----------------------------------------------//
struct abb {
    nodo_t *raiz;
    size_t cantidad;
    abb_comparar_clave_t cmp;
    abb_destruir_dato_t destruir_dato;
};

abb_t *abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato) {
    abb_t *abb = malloc(sizeof (abb_t));
    if (abb == NULL) return NULL;
    abb->raiz = NULL;
    abb->cantidad = 0;
    abb->cmp = cmp;
    abb->destruir_dato = destruir_dato;
    return abb;
}

size_t abb_cantidad(const abb_t *arbol) {
    return arbol->cantidad;
}

nodo_t** buscar_nodo(nodo_t **puntero_nodo, const char * clave, abb_comparar_clave_t cmp){
    if (*puntero_nodo == NULL){
        return puntero_nodo;
    } else if (cmp(clave, (*puntero_nodo)->clave) < 0){
        puntero_nodo = buscar_nodo(&(*puntero_nodo)->izq, clave, cmp);
    } else if (cmp(clave, (*puntero_nodo)->clave) > 0){
        puntero_nodo = buscar_nodo(&(*puntero_nodo)->der, clave, cmp);
    }

    return puntero_nodo;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato) {
    nodo_t** puntero_nodo = buscar_nodo(&arbol->raiz, clave, arbol->cmp);
    if (*puntero_nodo == NULL){
        *puntero_nodo = crear_nodo(clave, dato);
        if (*puntero_nodo == NULL) return false;
        arbol->cantidad++;

    } else {
        if (arbol->destruir_dato)
            arbol->destruir_dato((*puntero_nodo)->dato);
        (*puntero_nodo)->dato = dato;
    }
    return true;
}

bool abb_pertenece(const abb_t *arbol, const char *clave) {
    nodo_t *nodo = arbol->raiz;
    nodo_t** puntero_nodo = buscar_nodo(&nodo, clave, arbol->cmp);
    return *puntero_nodo != NULL;
}

void *abb_obtener(const abb_t *arbol, const char *clave) {
    nodo_t *nodo = arbol->raiz;
    nodo_t** puntero_nodo = buscar_nodo(&nodo, clave, arbol->cmp);
    return (*puntero_nodo) ? (*puntero_nodo)->dato : NULL;
}

void *abb_borrar(abb_t *arbol, const char *clave) {
    nodo_t** puntero_nodo = buscar_nodo(&arbol->raiz, clave, arbol->cmp);
    if (*puntero_nodo == NULL) return NULL;


    nodo_t *nodo = *puntero_nodo;

    if (nodo->izq == NULL && nodo->der == NULL){
        *puntero_nodo = NULL;
    } else if (nodo->izq !=NULL && nodo->der == NULL){
        *puntero_nodo = nodo->izq;
    } else if (nodo->der != NULL && nodo->izq == NULL){
        *puntero_nodo = nodo->der;
    } else if (nodo->izq != NULL && nodo->der != NULL){
        void* dato = (*puntero_nodo)->dato;
        nodo_t **nodoMaxIzq = buscar_nodo(&arbol->raiz, (*puntero_nodo)->izq->clave, arbol->cmp);
        nodo_t* reemplazante = *nodoMaxIzq;

        while (reemplazante->der != NULL){
            reemplazante = reemplazante->der;
        }

        char* nodo_max_izq_clave = strdup(reemplazante->clave);
        void* nodo_max_izq_dato = abb_borrar(arbol, nodo_max_izq_clave);

        free(nodo->clave);
        (*puntero_nodo)->clave = nodo_max_izq_clave;
        (*puntero_nodo)->dato = nodo_max_izq_dato;

        return dato;
    }
    void* dato = destruir_nodo(nodo);
    arbol->cantidad--;
    return dato;
}

bool inorder(nodo_t *nodo, bool f(const char *, void *, void *), void *extra){
    if (nodo == NULL) return true;
    bool seguir = inorder(nodo->izq, f, extra);
    if (seguir == false || !f(nodo->clave, nodo->dato, extra)) return false;
    seguir &= inorder(nodo->der, f, extra);
    return seguir;
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra){
    inorder(arbol->raiz, visitar, extra);
}

void destruir_postorder(abb_t *arbol, nodo_t *nodo){
    if (nodo == NULL) return;
    destruir_postorder(arbol, nodo->izq);
    destruir_postorder(arbol, nodo->der);

    void *dato = destruir_nodo(nodo);
    if (arbol->destruir_dato)
        arbol->destruir_dato(dato);

}

void abb_destruir(abb_t *arbol) {
    destruir_postorder(arbol, arbol->raiz);
    free(arbol);
}


//---------------ARBOL ITER------------------------//

struct abb_iter {
    pila_t *pila;
};

/* No entiendo para que esta funcion pero bueno */
void avanzar_izq(pila_t* pila, nodo_t *nodo){
    if (nodo->izq){
        pila_apilar(pila, nodo->izq);
        avanzar_izq(pila, nodo->izq);
    }
}

abb_iter_t *abb_iter_in_crear(const abb_t *arbol) {
    abb_iter_t *iter = malloc(sizeof (abb_iter_t));
    if (iter == NULL) return NULL;

    pila_t *pila = pila_crear();
    if (pila == NULL){
        free(iter);
        return NULL;
    }
    nodo_t *nodo = arbol->raiz;
    if (nodo){
        pila_apilar(pila, nodo);
        avanzar_izq(pila, nodo);
    }
    iter->pila = pila;
    return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter) {
    if (abb_iter_in_al_final(iter)){
        return false;
    }
    nodo_t *nodo = pila_desapilar(iter->pila);
    if (!nodo->der)
        return true;
    nodo = nodo->der;
    pila_apilar(iter->pila, nodo);

    avanzar_izq(iter->pila, nodo);
    return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter) {
    nodo_t *actual = pila_ver_tope(iter->pila);
    return (actual) ? actual->clave : NULL;
}

bool abb_iter_in_al_final(const abb_iter_t *iter) {
    return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t *iter) {
    pila_destruir(iter->pila);
    free(iter);
}
