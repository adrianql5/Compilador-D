#include "tablaHash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *duplicarCadena(const char *origen){
    size_t longitud = strlen(origen) + 1;
    char *copia = malloc(longitud);
    if(!copia){
        return NULL;
    }
    memcpy(copia, origen, longitud);
    return copia;
}
static unsigned long funcionHash(const char *str, int capacidad){
    unsigned long hash = 1469598103934665603UL;

    while(*str){
        hash ^= (unsigned char)*str++;
        hash *= 1099511628211UL;
    }

    return hash % capacidad;
}
static CompLexico *crearNodo(const char *lexema, int identificador){
    CompLexico *nodo = (CompLexico *)malloc(sizeof(CompLexico));
    if(!nodo) return NULL;

    (*nodo).lexema = duplicarCadena(lexema);
    if(!(*nodo).lexema){
        free(nodo);
        return NULL;
    }

    (*nodo).identificador = identificador;
    (*nodo).siguiente = NULL;
    return nodo;
}
TablaHash *crearTablaHash(int capacidad){
    if(capacidad == 0) capacidad = 16;

    TablaHash *th = (TablaHash *)malloc(sizeof(TablaHash));
    if(!th) return NULL;

    (*th).cajones = calloc(capacidad, sizeof(CompLexico *));
    if(!(*th).cajones){
        free(th);
        return NULL;
    }

    (*th).capacidad = capacidad;
    (*th).tamano = 0;
    (*th).factorCargaMax = 0.75f;
    return th;
}
static int cambiarTamano(TablaHash *th){
    int nuevaCapacidad = (*th).capacidad * 2;
    CompLexico **nuevosCajones = calloc(nuevaCapacidad, sizeof(CompLexico *));
    if(!nuevosCajones) return -1;

    for(int i = 0; i < (*th).capacidad; i++){
        CompLexico *nodo = *((*th).cajones + i);
        while(nodo){
            CompLexico *siguiente = (*nodo).siguiente;

            unsigned long nuevoIndice = funcionHash((*nodo).lexema, nuevaCapacidad);
            (*nodo).siguiente = *(nuevosCajones + nuevoIndice);
            *(nuevosCajones + nuevoIndice) = nodo;

            nodo = siguiente;
        }
    }

    free((*th).cajones);
    (*th).cajones = nuevosCajones;
    (*th).capacidad = nuevaCapacidad;
    return 0;
}
int insertarTablaHash(TablaHash *th, const char *lexema, int identificador){
    if(!th || !lexema) return -1;

    float factorCarga = (float)((*th).tamano + 1) / (float)(*th).capacidad;

    if(factorCarga > (*th).factorCargaMax)
        if(cambiarTamano(th) != 0) return -1;

    unsigned long indice = funcionHash(lexema, (*th).capacidad);

    CompLexico *actual = *((*th).cajones + indice);
    while(actual){
        if(strcmp((*actual).lexema, lexema) == 0){
            (*actual).identificador = identificador;
            return 0;
        }
        actual = (*actual).siguiente;
    }

    CompLexico *nuevoNodo = crearNodo(lexema, identificador);
    if(!nuevoNodo) return -1;

    (*nuevoNodo).siguiente = *((*th).cajones + indice);
    *((*th).cajones + indice) = nuevoNodo;
    (*th).tamano++;
    return 0;
}
CompLexico *buscarTablaHash(TablaHash *th, const char *lexema){
    if(!th || !lexema) return NULL;

    unsigned long indice = funcionHash(lexema, (*th).capacidad);

    CompLexico *actual = *((*th).cajones + indice);
    while(actual){
        if(strcmp((*actual).lexema, lexema) == 0){
            return actual;
        }
        actual = (*actual).siguiente;
    }
    return NULL;
}
int eliminarTablaHash(TablaHash *th, const char *lexema){
    if(!th || !lexema) return -1;

    unsigned long indice = funcionHash(lexema, (*th).capacidad);

    CompLexico *actual = *((*th).cajones + indice);
    CompLexico *previo = NULL;

    while(actual){
        if(strcmp((*actual).lexema, lexema) == 0){
            if(previo){
                (*previo).siguiente = (*actual).siguiente;
            } else {
                *((*th).cajones + indice) = (*actual).siguiente;
            }

            free((*actual).lexema);
            free(actual);
            (*th).tamano--;
            return 0;
        }
        previo = actual;
        actual = (*actual).siguiente;
    }
    return -1;
}
void imprimirTablaHash(TablaHash th){
    printf("=== Tabla Hash (tamano=%d, capacidad=%d) ===\n",
           th.tamano, th.capacidad);

    for(int i = 0; i < th.capacidad; i++){
        CompLexico *nodo = *(th.cajones + i);
        if(nodo){
            printf("  [%d]: ", i);
            while(nodo){
                printf("(\"%s\", %d) ", (*nodo).lexema, (*nodo).identificador);
                nodo = (*nodo).siguiente;
            }
            printf("\n");
        }
    }
}
void destruirTablaHash(TablaHash *th){
    if(!th) return;

    for(int i = 0; i < (*th).capacidad; i++){
        CompLexico *nodo = *((*th).cajones + i);
        while(nodo){
            CompLexico *siguiente = (*nodo).siguiente;
            free((*nodo).lexema);
            free(nodo);
            nodo = siguiente;
        }
    }

    free((*th).cajones);
    free(th);
}





















































