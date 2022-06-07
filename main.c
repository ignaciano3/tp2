#include "algogram.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void throw(const char *msg) {
    fprintf(stderr,"%s", msg);
    exit(1);
}

FILE* openfile(const char *file_name) {
    FILE *fptr;

    fptr = fopen(file_name,"r");

    if (fptr == NULL){
        throw("Error: archivo fuente inaccesible");
    }

    return fptr;
}

int main (int argc, char **argv){
    if (argc != 2) return 1;

    return 0;
}