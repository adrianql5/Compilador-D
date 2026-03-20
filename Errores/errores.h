#ifndef ERRORES_H
#define ERRORES_H

typedef enum {
    ERROR_SISTEMA_ENTRADA = 0,
    ERROR_LEXICO = 1
} TipoError;

/**
 * @brief Reporta un error con formato uniforme.
 * @param tipo Categoria del error.
 * @param mensaje Descripcion textual del problema.
 */
void reportarError(TipoError tipo, const char *mensaje);

#endif
