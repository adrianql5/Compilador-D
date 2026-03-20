#include "sintatico.h"

#include <stdio.h>


int inicializarAnalizadorSintactico(AnalizadorSintactico *sintactico, AnalizadorLexico *lexico){
    if(!sintactico || !lexico){
        return -1;
    }

    sintactico->lexico = lexico;
    return 0;
}


// Pedirá componentes lexicos hasta recibir un EOF 
void iniciarAnalisis(AnalizadorSintactico *sintactico){
    if(!sintactico || !sintactico->lexico){
        printf("No se pudo inicializar el analizador sintactico\n");
        return;
    }

    for(;;){
        TokenLexico token = sigCompLexico(sintactico->lexico);
        printf("< %d, %s >\n", token.identificador, token.lexema);

        if(token.identificador == EOF){
            break;
        }
    }
}

void cerrarAnalizadorSintactico(AnalizadorSintactico *sintactico){
    if(!sintactico){
        return;
    }

    sintactico->lexico = NULL;
}
