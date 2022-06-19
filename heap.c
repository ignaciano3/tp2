#include <stdlib.h>
#include "heap.h"
#define CAPACIDAD_STANDARD 10

void swap(void **a, void **b) {
    void* temp = *a;
    *a = *b;
    *b = temp;
}

struct heap {
    cmp_func_t cmp;
    size_t capacidad, cantidad;
    void** datos;
};

heap_t *heap_crear(cmp_func_t cmp) {
    heap_t* heap = malloc(sizeof (heap_t));
    if (heap == NULL) return NULL;
    heap->capacidad = CAPACIDAD_STANDARD;
    heap->cantidad = 0;
    heap->cmp = cmp;
    heap->datos = malloc (sizeof (void*) * CAPACIDAD_STANDARD);
    if (heap->datos == NULL){
        free(heap);
        return NULL;
    }
    return heap;
}

void heap_redimensionar_datos(heap_t *heap, size_t nuevaCapacidad){
    heap->datos = realloc(heap->datos, sizeof (void*) * nuevaCapacidad);
    if (heap->datos) heap->capacidad = nuevaCapacidad;
}

void downHeap(void** datos, cmp_func_t cmp, size_t cantidad, size_t padre){
    size_t h_izq = 2 * padre + 1;
    size_t h_der = 2 * padre + 2;
    if (h_izq >= cantidad) return;

    size_t min = h_izq;
    if (h_der < cantidad && cmp(datos[h_izq], datos[h_der]) < 0)
        min = h_der;

    if (cmp(datos[min], datos[padre]) > 0){
        swap(&datos[min], &datos[padre]);
        downHeap(datos, cmp, cantidad, min);
    }
}

void heapify(void **elementos, cmp_func_t cmp, size_t cant){
    for (int i = (int)cant/2-1; i>= 0; i--){
        downHeap(elementos, cmp, cant, (size_t)i);
    }
}

heap_t *heap_crear_arr(void **arreglo, size_t n, cmp_func_t cmp) {
    heap_t* heap = heap_crear(cmp);
    if (heap == NULL) return NULL;

    while (heap->capacidad < n){
        heap_redimensionar_datos(heap, heap->capacidad*2);
        if (!heap->datos){
            free(heap);
            return NULL;
        }
    }

    for (int i = 0; i < n; i++){
        heap->datos[i] = arreglo[i];
    }
    heap->cantidad = n;
    heapify(heap->datos, cmp, heap->cantidad);

    return heap;
}

size_t heap_cantidad(const heap_t *heap) {
    return heap->cantidad;
}

bool heap_esta_vacio(const heap_t *heap) {
    return heap->cantidad == 0;
}

void upHeap(void** datos, cmp_func_t cmp, size_t hijo){
    if (hijo == 0) return;
    size_t padre = (hijo-1)/2;
    if (cmp(datos[hijo], datos[padre]) > 0){
        swap(&datos[hijo], &datos[padre]);
        upHeap(datos, cmp, padre);
    }
}

bool heap_encolar(heap_t *heap, void *elem) {
    if (heap->cantidad == heap->capacidad){
        heap_redimensionar_datos(heap, heap->capacidad*2);
        if (!heap->datos) return NULL;
    }

    heap->datos[heap->cantidad] = elem;
    upHeap(heap->datos, heap->cmp, heap->cantidad);
    heap->cantidad++;
    return true;
}

void *heap_ver_max(const heap_t *heap) {
    return (!heap_esta_vacio(heap)) ? heap->datos[0] : NULL;
}

void *heap_desencolar(heap_t *heap) {
    if (heap_esta_vacio(heap)) return NULL;
    void* dato = heap->datos[0];

    heap->cantidad--;

    // Paso el ultimo al principio
    swap(&heap->datos[0], &heap->datos[heap->cantidad]);

    downHeap(heap->datos, heap->cmp, heap->cantidad, 0);

    if (heap->cantidad <= heap->capacidad/4 && heap->cantidad <= CAPACIDAD_STANDARD){
        heap_redimensionar_datos(heap, heap->capacidad/2);
        if (heap->datos == NULL) return NULL;
    }

    return dato;
}

void heap_destruir(heap_t *heap, void (*destruir_elemento)(void *)) {
    if (destruir_elemento){
        for (int i = 0; i < heap->cantidad; i++)
            destruir_elemento(heap->datos[i]);
    }
    free(heap->datos);
    free(heap);
}

void heap_sort(void **elementos, size_t cant, cmp_func_t cmp) {
    heapify(elementos, cmp, cant);
    for (int i = (int)cant - 1; i >= 0; i--) {
        swap(&elementos[0], &elementos[i]);
        downHeap(elementos, cmp, (size_t)i, 0);
    }
}