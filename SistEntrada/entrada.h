#ifndef ENTRADA_H
#define ENTRADA_H

#include <stddef.h>
#include <stdio.h>

#define ENTRADA_TAM_BLOQUE_POR_DEFECTO 256

typedef struct {
    FILE *archivo;

    int *buffer[2];
    size_t tamBloque;
    size_t bytesLeidos[2];
    unsigned char *bloqueTemporal;

    int *primero;
    int *delantero;
    int mitadDelantero;

    size_t longitudLexema;
    int capturandoLexema;
    int lexemaExcedido;
} SistemaEntrada;

/**
 * @brief Inicializa el sistema de entrada y abre el fichero fuente.
 * @param entrada Estructura de estado a inicializar.
 * @param ruta Ruta del fichero fuente.
 * @param tamBloque Tamano de bloque para cada mitad del doble buffer.
 * @return 0 si inicializa correctamente, -1 en error.
 */
int inicializarEntrada(SistemaEntrada *entrada, const char *ruta, size_t tamBloque);

/**
 * @brief Devuelve el siguiente caracter del flujo de entrada.
 * @param entrada Sistema de entrada activo.
 * @return Caracter leido (como int sin signo) o EOF al finalizar/error.
 */
int sigCaracter(SistemaEntrada *entrada);

/**
 * @brief Retrocede una posicion para releer el ultimo caracter consumido.
 * @param entrada Sistema de entrada activo.
 */
void retrocederCaracter(SistemaEntrada *entrada);

/**
 * @brief Marca el inicio del lexema actual a partir del primer caracter ya leido.
 * @param entrada Sistema de entrada activo.
 * @param primerCaracter Primer caracter del lexema ya consumido.
 */
void marcarInicioLexema(SistemaEntrada *entrada, int primerCaracter);

/**
 * @brief Copia y devuelve el lexema capturado desde la ultima marca de inicio.
 * @param entrada Sistema de entrada activo.
 * @return Cadena dinamica terminada en '\0' (el llamador debe liberar) o NULL.
 */
char *obtenerLexema(SistemaEntrada *entrada);

/**
 * @brief Cancela la captura del lexema actual.
 * @param entrada Sistema de entrada activo.
 */
void cancelarLexemaActual(SistemaEntrada *entrada);

/**
 * @brief Cierra fichero y libera buffers del sistema de entrada.
 * @param entrada Sistema de entrada a cerrar.
 */
void cerrarEntrada(SistemaEntrada *entrada);

#endif
