#ifndef TP2_ALGO_ALGOGRAM_H
#define TP2_ALGO_ALGOGRAM_H
#include <stdio.h>
typedef struct algogram algogram_t;

algogram_t *crear();
void login(algogram_t *algo, char* usuario);
void logout(algogram_t *algo);
void postear(algogram_t *algo);
void proximo_post(algogram_t *algo);
void likear(algogram_t *algo);
void ver_likes(algogram_t *algo);

//---------------------//
/*
 * Lanza un error con el mensaje indicado
 */
void throw(const char * msg);

/*
 * Abre un archivo de texto y verifica que sea accesible
 */

FILE* openfile(const char *file_name);

#endif //TP2_ALGO_ALGOGRAM_H
