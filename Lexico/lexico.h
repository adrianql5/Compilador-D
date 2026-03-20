#ifndef LEXICO_H
#define LEXICO_H

#include <stddef.h>

#include "SistEntrada/entrada.h"
#include "TablaSimbolos/TS.h"

typedef struct {
    int identificador;
    const char *lexema;
} TokenLexico;

typedef struct {
    SistemaEntrada *entrada;
    TablaHash *ts;
    char *lexemaActual;
} AnalizadorLexico;

/**
 * @brief Inicializa el analizador lexico con estructuras ya creadas.
 * @param lexico Analizador a inicializar.
 * @param entrada Sistema de entrada ya abierto.
 * @param ts Tabla de simbolos compartida.
 * @return 0 si la inicializacion fue correcta, -1 en error.
 */
int iniciarAnalizadorLexico(AnalizadorLexico *lexico, SistemaEntrada *entrada, TablaHash *ts);

/**
 * @brief Libera recursos propios del analizador lexico.
 * @param lexico Analizador a limpiar.
 */
void cerrarAnalizadorLexico(AnalizadorLexico *lexico);

/**
 * @brief Devuelve el siguiente componente lexico de la entrada.
 * @param lexico Analizador lexico activo.
 * @return Token reconocido; devuelve identificador EOF cuando termina la entrada.
 */
TokenLexico sigCompLexico(AnalizadorLexico *lexico);

#endif
