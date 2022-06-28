#include "pila.h"
#include <stdlib.h>

#define CAPACIDAD_STANDARD 10
#define FACTOR_AGRANDAR 2
#define FACTOR_ACHICAR (FACTOR_AGRANDAR * FACTOR_AGRANDAR)

/* Definición del struct pila proporcionado por la cátedra.
 */
struct pila {
    void **datos;
    size_t cantidad;   // Cantidad de elementos almacenados.
    size_t capacidad;  // Capacidad del arreglo 'datos'.
};

/* *****************************************************************
 *                    PRIMITIVAS DE LA PILA
 * *****************************************************************/

pila_t *pila_crear(void) {
    pila_t* pila = malloc(sizeof (pila_t));
    if (pila == NULL) return NULL;
    pila->cantidad = 0;
    pila->capacidad = CAPACIDAD_STANDARD;
    pila->datos = malloc(sizeof (void*) * pila->capacidad);
    if (pila->datos == NULL) {
        free(pila);
        return NULL;
    }
    return pila;
}

bool pila_esta_vacia(const pila_t *pila) {
    return pila->cantidad == 0;
}

void pila_redimensionar_datos(pila_t *pila, size_t nuevaCapacidad){
    pila->datos = realloc(pila->datos, sizeof (void*) * nuevaCapacidad);
    if (pila->datos)
        pila->capacidad = nuevaCapacidad;
}

bool pila_apilar(pila_t *pila, void *valor) {
    if (pila->cantidad == pila->capacidad){
        pila_redimensionar_datos(pila, pila->capacidad*FACTOR_AGRANDAR);
        if (pila->datos == NULL)
            return false;
    }
    pila->datos[pila->cantidad] = valor;
    pila->cantidad++;
    return true;
}

void *pila_ver_tope(const pila_t *pila) {
    if (pila_esta_vacia(pila)) return NULL;
    return pila->datos[pila->cantidad-1];
}

void *pila_desapilar(pila_t *pila) {
    if (pila_esta_vacia(pila)) return NULL;
    pila->cantidad--;
    if (pila->capacidad > CAPACIDAD_STANDARD && pila->cantidad * FACTOR_ACHICAR < pila->capacidad){
        pila_redimensionar_datos(pila, pila->capacidad/FACTOR_AGRANDAR);
        if (pila->datos == NULL)
            return NULL;
    }
    return pila->datos[pila->cantidad];
}

void pila_destruir(pila_t *pila) {
    free(pila->datos);
    free(pila);
}