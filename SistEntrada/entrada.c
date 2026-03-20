#include "entrada.h"
#include "Errores/errores.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Reenvia todos los errores del modulo al gestor comun de errores.
static void reportarErrorEntrada(const char *mensaje) {
    reportarError(ERROR_SISTEMA_ENTRADA, mensaje);
}

// Cuenta la longitud del lexema actual y detecta si supera el tamano maximo.
static void registrarCaracterLexema(SistemaEntrada *entrada) {
    if (!entrada || !entrada->capturandoLexema) {
        return;
    }

    if (entrada->lexemaExcedido) {
        return;
    }

    if (entrada->longitudLexema >= entrada->tamBloque) {
        reportarErrorEntrada("El lexema supera el tamano maximo de bloque");
        entrada->lexemaExcedido = 1;
        return;
    }
    entrada->longitudLexema++;
}

// Carga una mitad del doble buffer desde el fichero y coloca su centinela EOF.
static int cargarMitad(SistemaEntrada *entrada, int mitad) {
    size_t bytesLeidos;

    if (!entrada || !entrada->archivo || !entrada->buffer[mitad] || !entrada->bloqueTemporal) {
        return -1;
    }

    clearerr(entrada->archivo);
    bytesLeidos = fread(entrada->bloqueTemporal, sizeof(unsigned char), entrada->tamBloque, entrada->archivo);
    if (ferror(entrada->archivo)) {
        return -1;
    }

    for (size_t i = 0; i < bytesLeidos; i++) {
        entrada->buffer[mitad][i] = entrada->bloqueTemporal[i];
    }

    entrada->buffer[mitad][bytesLeidos] = EOF;
    entrada->bytesLeidos[mitad] = bytesLeidos;
    return 0;
}

// Abre el fichero y deja listo el doble buffer para empezar a leer caracteres.
int inicializarEntrada(SistemaEntrada *entrada, const char *ruta, size_t tamBloque) {
    if (!entrada || !ruta) {
        return -1;
    }

    *entrada = (SistemaEntrada){0};

    if (tamBloque == 0) {
        tamBloque = ENTRADA_TAM_BLOQUE_POR_DEFECTO;
    }

    entrada->archivo = fopen(ruta, "r");
    if (!entrada->archivo) {
        reportarErrorEntrada("No se pudo abrir el archivo de entrada");
        return -1;
    }

    entrada->buffer[0] = malloc((tamBloque + 1) * sizeof(int));
    entrada->buffer[1] = malloc((tamBloque + 1) * sizeof(int));
    entrada->bloqueTemporal = malloc(tamBloque * sizeof(unsigned char));
    if (!entrada->buffer[0] || !entrada->buffer[1] || !entrada->bloqueTemporal) {
        reportarErrorEntrada("No se pudo reservar memoria para los buffers de entrada");
        cerrarEntrada(entrada);
        return -1;
    }

    entrada->tamBloque = tamBloque;

    if (cargarMitad(entrada, 0) != 0) {
        reportarErrorEntrada("No se pudo leer el primer bloque de entrada");
        cerrarEntrada(entrada);
        return -1;
    }

    entrada->buffer[1][0] = EOF;
    entrada->bytesLeidos[1] = 0;
    entrada->mitadDelantero = 0;
    entrada->delantero = entrada->buffer[0];
    return 0;
}

// Devuelve el siguiente caracter del fichero recargando mitades del buffer si hace falta.
int sigCaracter(SistemaEntrada *entrada) {
    int c;
    int siguienteMitad;

    // Si el sistema no esta bien inicializado no se puede seguir leyendo.
    if (!entrada || !entrada->archivo || !entrada->delantero) {
        reportarErrorEntrada("Estado invalido al pedir el siguiente caracter");
        return EOF;
    }

    while (1) {
        c = *entrada->delantero;
        // Si no estamos en el centinela, devolvemos el caracter y avanzamos.
        if (c != EOF) {
            entrada->delantero++;
            registrarCaracterLexema(entrada);
            return c;
        }

        // Si esta mitad tenia menos de tamBloque bytes, este EOF es fin real de fichero.
        if (entrada->bytesLeidos[entrada->mitadDelantero] < entrada->tamBloque) {
            return EOF;
        }

        siguienteMitad = 1 - entrada->mitadDelantero;
        // Si la otra mitad no se puede recargar, se informa del error y se corta la lectura.
        if (cargarMitad(entrada, siguienteMitad) != 0) {
            reportarErrorEntrada("Error de lectura al recargar el buffer de entrada");
            return EOF;
        }

        entrada->mitadDelantero = siguienteMitad;
        entrada->delantero = entrada->buffer[siguienteMitad];
    }
}

// Deshace la ultima lectura para que el siguiente sigCaracter relea ese caracter.
void retrocederCaracter(SistemaEntrada *entrada) {
    int mitadAnterior;

    // Sin un estado valido no tiene sentido retroceder.
    if (!entrada || !entrada->archivo || !entrada->delantero) {
        reportarErrorEntrada("No se puede retroceder caracter en el estado actual");
        return;
    }

    // Si seguimos dentro de la misma mitad, basta con mover el puntero una posicion atras.
    if (entrada->delantero > entrada->buffer[entrada->mitadDelantero]) {
        entrada->delantero--;
    } else {
        mitadAnterior = 1 - entrada->mitadDelantero;
        // Si la mitad anterior no tiene datos, no hay ningun caracter valido al que volver.
        if (entrada->bytesLeidos[mitadAnterior] == 0) {
            reportarErrorEntrada("No se puede retroceder antes del inicio del buffer");
            return;
        }

        entrada->mitadDelantero = mitadAnterior;
        // Te lleva al bloque anterior + todos los bytes del bloque menos el centinela
        entrada->delantero = entrada->buffer[mitadAnterior] + entrada->bytesLeidos[mitadAnterior] - 1;
    }

    if (entrada->capturandoLexema && !entrada->lexemaExcedido && entrada->longitudLexema > 0) {
        entrada->longitudLexema--;
    }
}

// Marca el caracter ya leido como inicio del lexema actual.
void marcarInicioLexema(SistemaEntrada *entrada, int primerCaracter) {
    if (!entrada || !entrada->archivo || primerCaracter == EOF) {
        return;
    }

    entrada->primero = entrada->delantero - 1;
    entrada->longitudLexema = 0;
    entrada->capturandoLexema = 1;
    entrada->lexemaExcedido = 0;
    registrarCaracterLexema(entrada);
}

// Reconstruye y devuelve el lexema actual recorriendo el buffer desde primero.
char *obtenerLexema(SistemaEntrada *entrada) {
    char *lexema;
    int *cursor;
    int mitadCursor;

    if (!entrada || !entrada->archivo || !entrada->capturandoLexema) {
        reportarErrorEntrada("Estado invalido al obtener lexema");
        return NULL;
    }

    if (entrada->lexemaExcedido) {
        cancelarLexemaActual(entrada);
        return NULL;
    }

    lexema = malloc(entrada->longitudLexema + 1);
    if (!lexema) {
        reportarErrorEntrada("No se pudo reservar memoria para el lexema");
        return NULL;
    }

    cursor = entrada->primero;
    if (cursor >= entrada->buffer[0] && cursor <= entrada->buffer[0] + entrada->tamBloque) {
        mitadCursor = 0;
    } else if (cursor >= entrada->buffer[1] && cursor <= entrada->buffer[1] + entrada->tamBloque) {
        mitadCursor = 1;
    } else {
        free(lexema);
        reportarErrorEntrada("El inicio del lexema no apunta a un buffer valido");
        cancelarLexemaActual(entrada);
        return NULL;
    }

    for (size_t i = 0; i < entrada->longitudLexema; i++) {
        if (*cursor == EOF) {
            mitadCursor = 1 - mitadCursor;
            cursor = entrada->buffer[mitadCursor];
        }
        lexema[i] = (char)*cursor;
        cursor++;
    }
    lexema[entrada->longitudLexema] = '\0';

    cancelarLexemaActual(entrada);
    return lexema;
}

// Borra el estado temporal asociado al lexema que se estaba capturando.
void cancelarLexemaActual(SistemaEntrada *entrada) {
    if (!entrada) {
        return;
    }

    entrada->capturandoLexema = 0;
    entrada->longitudLexema = 0;
    entrada->lexemaExcedido = 0;
    entrada->primero = NULL;
}

// Cierra el fichero y libera toda la memoria reservada por el sistema de entrada.
void cerrarEntrada(SistemaEntrada *entrada) {
    if (!entrada) {
        return;
    }

    if (entrada->archivo) {
        fclose(entrada->archivo);
    }

    free(entrada->buffer[0]);
    free(entrada->buffer[1]);
    free(entrada->bloqueTemporal);
    *entrada = (SistemaEntrada){0};
}
