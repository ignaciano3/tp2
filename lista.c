#include "lista.h"
#include <stdlib.h>
#include <stddef.h>

//Nodo
typedef struct nodo {
    struct nodo* sig;
    void* dato;
} nodo_t;

nodo_t* nodo_lista_crear(void* dato){
    nodo_t* nodo = malloc(sizeof (nodo_t));
    if (nodo == NULL) return NULL;

    nodo->dato = dato;
    nodo->sig = NULL;
    return nodo;
}

//----------------------------------------------------------------------------//

// Lista
struct lista{
    nodo_t *prim;
    nodo_t *ult;
    size_t largo;
};

lista_t* lista_crear(){
    lista_t *lista = malloc(sizeof (lista_t));
    if (lista == NULL) return NULL;

    lista->prim = NULL;
    lista->ult = NULL;
    lista->largo = 0;
    return lista;
}

bool lista_esta_vacia(const lista_t* lista){
    return lista->prim ? false : true;
}

bool lista_insertar_primero(lista_t *lista, void *dato){
    nodo_t* nodo = nodo_lista_crear(dato);
    if (nodo == NULL) return NULL;

    nodo->sig = lista->prim;
    if (lista_esta_vacia(lista)){
        lista->ult = nodo;
    }
    lista->prim = nodo;


    lista->largo++;
    return true;
}

bool lista_insertar_ultimo(lista_t* lista, void* dato){
    nodo_t* nodo = nodo_lista_crear(dato);
    if (nodo == NULL) return NULL;

    if (lista_esta_vacia(lista)) {
        lista->prim = nodo;
        lista->ult = nodo;
    } else {
        lista->ult->sig = nodo;
        lista->ult = nodo;
    }

    lista->largo++;
    return true;
}

void *lista_borrar_primero(lista_t *lista){
    if (lista_esta_vacia(lista)) return NULL;
    nodo_t* nodo_a_borrar = lista->prim;

    void* dato = lista->prim->dato;
    lista->prim = lista->prim->sig;

    free(nodo_a_borrar);
    lista->largo--;
    return dato;
}

void *lista_ver_primero(const lista_t *lista){
    return (!lista_esta_vacia(lista)) ? lista->prim->dato : NULL;
}

void *lista_ver_ultimo(const lista_t* lista){
    return (!lista_esta_vacia(lista)) ? lista->ult->dato : NULL;
}

size_t lista_largo(const lista_t *lista){
    return lista->largo;
}

void lista_destruir(lista_t *lista, void (*destruir_dato)(void *)) {
    while (!lista_esta_vacia(lista)){
        void* prim = lista_borrar_primero(lista);
        if (destruir_dato != NULL) {
            destruir_dato(prim);
        }
    }
    free(lista);
}

void lista_iterar(lista_t* lista, bool (*func) (void*, void*), void* extra){
    nodo_t *nodo_iter = lista->prim;
    while (nodo_iter != NULL){
        if (!func(nodo_iter->dato, extra)){
            break;
        }
        nodo_iter = nodo_iter->sig;
    }
}

// --------------------------------------------------------------------- //
// --------------------------------------------------------------------- //

//Lista Iter
struct lista_iter{
    nodo_t* actual;
    lista_t* lista;
    nodo_t* anterior;
};

lista_iter_t* lista_iter_crear(lista_t* lista){
    if (lista == NULL) return NULL;
    lista_iter_t *iter = malloc(sizeof (lista_iter_t));
    if (iter == NULL) return NULL;

    iter->lista = lista;
    iter->actual = iter->lista->prim;
    iter->anterior = NULL;
    return iter;
}

bool lista_iter_avanzar(lista_iter_t* iter){
    if (lista_iter_al_final(iter)) return false;

    iter->anterior = iter->actual;
    iter->actual = iter->actual->sig;
    return true;
}

bool lista_iter_al_final(const lista_iter_t* iter){
    return iter->actual == NULL;
}

void* lista_iter_ver_actual(const lista_iter_t* iter){
    return (!lista_iter_al_final(iter)) ? iter->actual->dato : NULL;
}

void lista_iter_destruir(lista_iter_t* iter){
    free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){
    nodo_t* insertar = nodo_lista_crear(dato);
    if (insertar == NULL) return NULL;
    nodo_t* aux = iter->actual;

    //El actual pasa a ser el nodo que se agrega
    iter->actual = insertar;

    if (aux == NULL){
        iter->lista->ult = iter->actual;
    }

    if (iter->anterior != NULL){
        iter->anterior->sig = iter->actual;
    }

    //El siguiente es el que antes era el actual
    iter->actual->sig = aux;

    // Veo si el prim se me quedo adelantado
    if (aux == iter->lista->prim){
        iter->lista->prim = iter->actual;
    }

    iter->lista->largo++;
    return true;
}

void *lista_iter_borrar(lista_iter_t *iter){
    if (lista_iter_al_final(iter)) return NULL;

    nodo_t* nodo_a_borrar = iter->actual;

    //Veo si elimina al prim
    if (nodo_a_borrar == iter->lista->prim){
        iter->lista->prim = iter->lista->prim->sig;
    }

    // Tengo que ver si elimina al ult
    if (nodo_a_borrar == iter->lista->ult){
        iter->lista->ult = iter->anterior;
        if (iter->lista->ult != NULL)
            iter->lista->ult->sig = NULL;
    }

    iter->actual = iter->actual->sig;
    if (iter->anterior != NULL)
        iter->anterior->sig = iter->actual;
    iter->lista->largo--;

    void* dato = nodo_a_borrar->dato;
    free(nodo_a_borrar);
    return dato;
}
