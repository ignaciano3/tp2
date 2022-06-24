//
// Created by Bangho on 22/03/2022.
//

#ifndef LISTA_LISTA_H
#define LISTA_LISTA_H

#include <stdbool.h>
#include <stdlib.h>

struct lista;
typedef struct lista lista_t;

struct lista_iter;
typedef struct lista_iter lista_iter_t;

// Lista
lista_t* lista_crear();
bool lista_esta_vacia(const lista_t* lista);
bool lista_insertar_primero(lista_t *lista, void *dato);
bool lista_insertar_ultimo(lista_t* lista, void* dato);
void *lista_borrar_primero(lista_t *lista);
void *lista_ver_primero(const lista_t *lista);
void *lista_ver_ultimo(const lista_t* lista);
size_t lista_largo(const lista_t *lista);
void lista_destruir(lista_t* lista, void (*destruir_dato)(void *));
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);


// Iterador de lista
lista_iter_t* lista_iter_crear(lista_t* lista);
bool lista_iter_avanzar(lista_iter_t* iter);
void* lista_iter_ver_actual(const lista_iter_t* iter);
bool lista_iter_al_final(const lista_iter_t* iter);
void lista_iter_destruir(lista_iter_t* iter);
bool lista_iter_insertar(lista_iter_t *iter, void *dato);
void *lista_iter_borrar(lista_iter_t *iter);

// Las funciones que provee la catedra
void imprimir_iter_externo(lista_t *lista);
bool imprimir_un_item(void *elemento, void *extra);
void imprimir_iter_interno(lista_t *lista);
void ejemplo_iteradores();

void pruebas_lista_estudiante(void);

#endif //LISTA_LISTA_H
