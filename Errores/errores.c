#include "errores.h"

#include <stdio.h>

static const char *nombreTipoError(TipoError tipo){
    switch(tipo){
    case ERROR_SISTEMA_ENTRADA:
        return "SISTEMA_ENTRADA";
    case ERROR_LEXICO:
        return "LEXICO";
    default:
        return "DESCONOCIDO";
    }
}

void reportarError(TipoError tipo, const char *mensaje){
    if(!mensaje){
        mensaje = "Error no especificado";
    }

    fprintf(stderr, "[%s] %s\n", nombreTipoError(tipo), mensaje);
}
