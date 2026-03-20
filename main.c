#include "TablaSimbolos/TS.h"
#include "Sintactico/sintatico.h"
#include "SistEntrada/entrada.h"
#include <stdio.h>

int main(void){
    const char *rutaEntrada = "regression.d";
    size_t tamBloqueEntrada = ENTRADA_TAM_BLOQUE_POR_DEFECTO;
    TablaHash *ts = NULL;
    SistemaEntrada entrada = {0};
    AnalizadorLexico lexico = {0};
    AnalizadorSintactico sintactico = {0};

    //Inicializo la Tabla de Símbolos y la imprimo par aver que la precarga no falló 
    ts = TSinicializar();
    if(!ts){
        printf("Error al crear la tabla de simbolos\n");
        return 1;
    }

    TSimprimir(ts);

    // inicializo el sistema de entrada con la ruta al archivo y el tamaño de bloque 
    if(inicializarEntrada(&entrada, rutaEntrada, tamBloqueEntrada) != 0){
        printf("No se pudo inicializar el sistema de entrada: %s\n", rutaEntrada);
        TSdestruir(ts);
        return 1;
    }

    // inicio el analizador léxico, pasandole el sistema de entrada con que se va a comunicar y la tabla de símbolos 
    if(iniciarAnalizadorLexico(&lexico, &entrada, ts) != 0){
        printf("No se pudo inicializar el analizador lexico\n");
        cerrarEntrada(&entrada);
        TSdestruir(ts);
        return 1;
    }

    // Inicio el analizador sintáctico pasándole el analizador lexico con el que va a hablar 
    if(inicializarAnalizadorSintactico(&sintactico, &lexico) != 0){
        printf("No se pudo inicializar el analizador sintactico\n");
        cerrarAnalizadorLexico(&lexico);
        cerrarEntrada(&entrada);
        TSdestruir(ts);
        return 1;
    }

    // Inicio el análisis 
    iniciarAnalisis(&sintactico);

    // Imprimo la TS para ver si inserte bien todos los ids 
    TSimprimir(ts);

    // Libero recursos
    cerrarAnalizadorSintactico(&sintactico);
    cerrarAnalizadorLexico(&lexico);
    cerrarEntrada(&entrada);
    TSdestruir(ts);
    return 0;
}
