#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "hash.h"
#include <ctype.h>
#define TAM 113
#define OCUPADO 1
#define VACIO 2
#define BORRADO 3
#define PORCEN_REDIM 70
#define AGRANDAR 2
#define PORCEN_ACHICAR 15
#define ACHICAR 1/2

typedef struct campo{
    const char* clave;
    void* dato;
    int estado;
} campo_t;

typedef struct hash{
    campo_t* tabla;
    size_t tam;
    size_t cant;
    hash_destruir_dato_t destruir_dato;
   
} hash_t;

bool _resolucion_colisiones(int estado,int estado2, unsigned int *hasheado, const hash_t *hash, const char *clave){
    while ((hash->tabla[*hasheado].estado != estado) && (hash->tabla[*hasheado].estado != estado2)){
        if ((char*)(hash->tabla[*hasheado].clave) && strcmp((char*)(hash->tabla[*hasheado].clave), (char*)clave) == 0){
            return true;
        }
        *hasheado += 1;
        *hasheado = *hasheado % (unsigned int)hash->tam;
    }
    return false;
}

void _declarar_campos(hash_t * hash){
    for (int i = 0; i < ((hash->tam)); i++){
        hash->tabla[i].estado = VACIO;
        hash->tabla[i].clave = NULL;
    }
}

void _redimensionar(hash_t *hash, size_t nuevo_tam, size_t nueva_memoria){
    campo_t *tabla = hash->tabla;
    campo_t *nueva_tabla = (campo_t*)malloc(nueva_memoria);
    if (nueva_tabla == NULL){
        return;
    }
    size_t taman = hash->tam;
    hash->tam = nuevo_tam;
    hash->tabla = nueva_tabla;
    hash->cant = 0;
    
    _declarar_campos(hash);
    
    for (int i = 0; i < taman; i++){
        if ((tabla[i].estado == OCUPADO)){
            hash_guardar(hash, tabla[i].clave, tabla[i].dato);
            free((char*)tabla[i].clave);
        }
    }
    free(tabla);  
}
    
char* _copiar_clave(const char *clave){
    char* copia = (char*)malloc((strlen(clave)) * sizeof(char) + 1);
    if (copia == NULL){
        return NULL;
    }
    strcpy(copia, clave);
    return copia;
}

unsigned int hashing(const char *word){
    //djb2 https://stackoverflow.com/questions/64699597/how-to-write-djb2-hashing-function-in-c
    unsigned int hashing = 5381;
    int c;

    while ((c = *word++)){       // *str++ is going to the next address in memory, where the next char in the string is stored
    
        if (isupper(c))
        {
            c = c + 32;
        }

        hashing = ((hashing << 5) + hashing) + c; // hash * 33 + c   // hash << 5 = hash * 2^5
    }

    return hashing;
}

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t *hash = (hash_t*)malloc(sizeof(hash_t));
    if (hash == NULL){
        return NULL;
    }
    hash->tabla = (campo_t*)malloc(TAM * (sizeof(campo_t)));
    if (hash->tabla == NULL){
        return NULL;
    }
    hash->cant = 0;
    hash->tam =  TAM;
    hash->destruir_dato = destruir_dato;
    _declarar_campos(hash);
    return hash;

}

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    char* copia = _copiar_clave(clave);
    if (copia == NULL){
        return false;
    }
    int porc = (int)(((float)(hash->cant) / (float)hash->tam) * 100);
    if (porc > PORCEN_REDIM){
        int factor = AGRANDAR;
        _redimensionar(hash, factor * hash->tam, (factor * hash->tam * sizeof(campo_t)));
    }
    unsigned int hasheado = (hashing(copia)) % ((unsigned int)(hash->tam));

    _resolucion_colisiones(VACIO, BORRADO, &hasheado, hash, clave);
    

    if (hash->tabla[hasheado].clave && (strcmp((char*)(hash->tabla[hasheado].clave), (char*)clave) == 0)){
        hash->cant--;
        if (hash->destruir_dato != NULL){
            hash->destruir_dato(hash->tabla[hasheado].dato); 
        }
    }
    free((char*)(hash->tabla[hasheado].clave));
    hash->cant++;
    hash->tabla[hasheado].clave = (const char*)copia;
    hash->tabla[hasheado].dato = dato;
    hash->tabla[hasheado].estado = OCUPADO;
    return true;

}

void *hash_borrar(hash_t *hash, const char *clave){
    unsigned int hasheado = (hashing(clave)) % ((unsigned int)(hash->tam));
    if (((hash->cant / hash->tam) * 100 ) == PORCEN_ACHICAR){
        int factor = ACHICAR;
        _redimensionar(hash, (size_t)(factor * (hash->tam)),(size_t)(factor * hash->tam * sizeof(campo_t)));
    }
    bool resolucion = _resolucion_colisiones(VACIO, false, &hasheado, hash, clave);
    if (resolucion){
        free((char*)(hash->tabla[hasheado].clave));
        hash->cant--;
        hash->tabla[hasheado].estado = BORRADO;
        hash->tabla[hasheado].clave = NULL;
        return hash->tabla[hasheado].dato;
    }
    return NULL;

}

void *hash_obtener(const hash_t *hash, const char *clave){
    unsigned int hasheado = (hashing(clave)) % ((unsigned int)(hash->tam));  
    return _resolucion_colisiones(VACIO, false, &hasheado,hash, clave) ? hash->tabla[hasheado].dato : NULL;
}

bool hash_pertenece(const hash_t *hash, const char *clave){
    unsigned int hasheado = (hashing(clave)) % ((unsigned int)(hash->tam));    
    return _resolucion_colisiones(VACIO, false, &hasheado, hash, clave);  
}

size_t hash_cantidad(const hash_t *hash){
    return hash->cant;
}

void hash_destruir(hash_t *hash){
    for (int i = 0; i < hash->tam; i++){
        if ((hash->tabla[i].estado == OCUPADO) && (hash->destruir_dato != NULL)){
            hash->destruir_dato(hash->tabla[i].dato);
        }
        free((char*)hash->tabla[i].clave);
        
    }
    free(hash->tabla);
    free(hash);
}


////////////////////////////////////////////
// HASH ITER
////////////////////////////////////////////

struct hash_iter{
    hash_t* hash;
    size_t pos;
};

void _siguiente_pos(const hash_t *hash, hash_iter_t *iter){
    for (int i = 0; i <= hash->tam; i++){
        if ((iter->pos == hash->tam) || (hash->tabla[iter->pos].estado == OCUPADO)){
            break;
        }
        iter->pos++;

    }
}

hash_iter_t *hash_iter_crear(const hash_t *hash){
    if (hash == NULL){
        return NULL;
    }
    hash_iter_t *iter = malloc(sizeof (hash_iter_t));
    if (iter == NULL){
        return NULL;
    }
    iter->pos = 0;
    iter->hash = (hash_t*)hash;

    _siguiente_pos(hash, iter);

    return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
    if (hash_iter_al_final(iter)){
        return false;
    }
    iter->pos ++;
    _siguiente_pos(iter->hash, iter);
    return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
    if (hash_iter_al_final(iter))
        return NULL;
    return iter->hash->tabla[iter->pos].clave;
}

bool hash_iter_al_final(const hash_iter_t *iter){
    return iter->pos == iter->hash->tam;
}

void hash_iter_destruir(hash_iter_t *iter){
    free(iter);
}



