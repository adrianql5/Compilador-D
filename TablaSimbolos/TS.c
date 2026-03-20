#include "TS.h"
#include "definiciones.h"
#include <stddef.h>
#include <ctype.h>
#include <stdio.h>


typedef struct {
    const char *lexema;
    int identificador;
} PalabraReservada;

// Así ya puedo precargar todo de forma cómodo
static PalabraReservada palabrasReservadas[] = {
    {"import",  IMPORT},
    {"double",  DOUBLE},
    {"int",     INT},
    {"while",   WHILE},
    {"foreach", FOREACH},
    {"return",  RETURN},
    {"cast",    CAST},
    {"void",    VOID},
    {NULL,      0}
};


TablaHash *TSinicializar(void){
    //Le meto tamaño 64 inicial, si hay muchas colisiones se va duplicando que es dinámica
    TablaHash *ts = crearTablaHash(64);
    if(!ts) return NULL;

    for(int i = 0; palabrasReservadas[i].lexema != NULL; i++){
        //inserto en la tabla cada palabra reservada
        insertarTablaHash(
            ts,
            palabrasReservadas[i].lexema,
            palabrasReservadas[i].identificador
        );
    }

    return ts;
}


CompLexico *TSbuscar(TablaHash *ts, const char *lexema){
    CompLexico *simbolo;

    if(!ts || !lexema){
        return NULL;
    }

    //Si encuentro el símbolo lo devuelvo 
    simbolo = buscarTablaHash(ts, lexema);
    if(simbolo){
        return simbolo;
    }

    // si no, lo inserto y lo devuelvo 
    if(isalpha((unsigned char)lexema[0]) || lexema[0] == '_'){
        if(insertarTablaHash(ts, lexema, ID) != 0){
            return NULL;
        }
        return buscarTablaHash(ts, lexema);
    }

    return NULL;
}


void TSimprimir(TablaHash *ts){
    if(!ts) return;
    printf("\n=== TABLA DE SIMBOLOS ===\n");
    imprimirTablaHash(*ts);  }

void TSdestruir(TablaHash *ts){
    destruirTablaHash(ts);
}
