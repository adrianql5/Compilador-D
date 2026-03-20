#ifndef TABLAHASH_H
#define TABLAHASH_H

typedef struct CompLexico{
    char *lexema;
    int identificador;
    struct CompLexico *siguiente;
} CompLexico;

typedef struct{
    CompLexico **cajones;
    int tamano;
    int capacidad;
    float factorCargaMax;
} TablaHash;

/**
 * @brief Crea una tabla hash vacia.
 * @param capacidad Capacidad inicial de cajones (si es 0 se usa valor por defecto).
 * @return Puntero a tabla creada o NULL en error.
 */
TablaHash *crearTablaHash(int capacidad);

/**
 * @brief Libera completamente una tabla hash.
 * @param th Tabla a destruir.
 */
void destruirTablaHash(TablaHash *th);

/**
 * @brief Inserta o actualiza un lexema en la tabla.
 * @param th Tabla destino.
 * @param lexema Lexema a insertar.
 * @param identificador Identificador asociado.
 * @return 0 si ok, -1 en error.
 */
int insertarTablaHash(TablaHash *th, const char *lexema, int identificador);

/**
 * @brief Busca un lexema exacto en la tabla.
 * @param th Tabla donde buscar.
 * @param lexema Lexema a buscar.
 * @return Simbolo encontrado o NULL si no existe/error.
 */
CompLexico *buscarTablaHash(TablaHash *th, const char *lexema);

/**
 * @brief Elimina un lexema de la tabla.
 * @param th Tabla donde eliminar.
 * @param lexema Lexema a eliminar.
 * @return 0 si elimina, -1 si no existe/error.
 */
int eliminarTablaHash(TablaHash *th, const char *lexema);

/**
 * @brief Imprime el contenido de la tabla hash.
 * @param th Tabla a imprimir.
 */
void imprimirTablaHash(TablaHash th);

#endif
