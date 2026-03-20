#ifndef SINTACTICO_H
#define SINTACTICO_H

#include "Lexico/lexico.h"

typedef struct {
    AnalizadorLexico *lexico;
} AnalizadorSintactico;

/**
 * @brief Inicializa el analizador sintactico con un lexico ya creado.
 * @param sintactico Analizador a inicializar.
 * @param lexico Analizador lexico activo.
 * @return 0 si la inicializacion fue correcta, -1 en error.
 */
int inicializarAnalizadorSintactico(AnalizadorSintactico *sintactico, AnalizadorLexico *lexico);

/**
 * @brief Punto de entrada del analizador sintactico.
 * @param sintactico Analizador sintactico activo.
 */
void iniciarAnalisis(AnalizadorSintactico *sintactico);

/**
 * @brief Limpia el estado del analizador sintactico.
 * @param sintactico Analizador a limpiar.
 */
void cerrarAnalizadorSintactico(AnalizadorSintactico *sintactico);

#endif
