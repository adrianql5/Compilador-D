#ifndef TS_H
#define TS_H

#include "tablaHash.h"

/**
 * @brief Crea la tabla de simbolos y precarga palabras reservadas.
 * @return Puntero a la tabla creada o NULL si falla.
 */
TablaHash *TSinicializar(void);

/**
 * @brief Busca un lexema en la tabla y lo inserta como ID si procede.
 * @param ts Tabla de simbolos.
 * @param lexema Cadena a resolver.
 * @return Simbolo completo (identificador + lexema canonico) o NULL en error.
 */
CompLexico *TSbuscar(TablaHash *ts, const char *lexema);

/**
 * @brief Imprime el contenido de la tabla de simbolos.
 * @param ts Tabla de simbolos a imprimir.
 */
void TSimprimir(TablaHash *ts);

/**
 * @brief Libera toda la memoria de la tabla de simbolos.
 * @param ts Tabla a destruir.
 */
void TSdestruir(TablaHash *ts);

#endif
