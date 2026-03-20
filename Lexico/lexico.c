#include "lexico.h"

#include "Errores/errores.h"
#include "SistEntrada/entrada.h"
#include "TablaSimbolos/definiciones.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Engancha el lexico con el sistema de entrada y la tabla de simbolos.
int iniciarAnalizadorLexico(AnalizadorLexico *lexico, SistemaEntrada *entrada, TablaHash *ts){
    if(!lexico || !entrada || !ts){
        return -1;
    }

    if(!entrada->archivo){
        return -1;
    }

    lexico->entrada = entrada;
    lexico->ts = ts;
    lexico->lexemaActual = NULL;
    return 0;
}

// Suelta la memoria temporal que usa el lexico entre tokens.
void cerrarAnalizadorLexico(AnalizadorLexico *lexico){
    if(!lexico){
        return;
    }

    free(lexico->lexemaActual);
    lexico->lexemaActual = NULL;
    lexico->entrada = NULL;
    lexico->ts = NULL;
}

// Construye el token tal cual, sin mas historia.
static TokenLexico crearToken(int identificador, const char *lexema){
    TokenLexico token;
    token.identificador = identificador;
    token.lexema = lexema;
    return token;
}

static void limpiarTokenActual(AnalizadorLexico *lexico){
    free(lexico->lexemaActual);
    lexico->lexemaActual = NULL;
    // Resetea en entrada.c la captura del token que estaba en curso.
    cancelarLexemaActual(lexico->entrada);
}

// Cierra un identificador o palabra reservada, reconstruye el lexema y lo
// resuelve en la tabla de simbolos.
static TokenLexico cerrarYResolverEnTS(AnalizadorLexico *lexico, int c){
    char *lexema;
    CompLexico *simbolo;

    if(c != EOF){
        retrocederCaracter(lexico->entrada);
    }

    lexema = obtenerLexema(lexico->entrada);
    if(!lexema){
        limpiarTokenActual(lexico);
        return sigCompLexico(lexico);
    }

    simbolo = TSbuscar(lexico->ts, lexema);
    free(lexema);

    if(!simbolo){
        reportarError(ERROR_LEXICO, "No se pudo resolver el lexema en la tabla de simbolos");
        limpiarTokenActual(lexico);
        return sigCompLexico(lexico);
    }

    return crearToken(simbolo->identificador, simbolo->lexema);
}

// Se come los espacios antes de empezar a reconocer el siguiente token.
static void ignorarBlancos(AnalizadorLexico *lexico){
    int c = sigCaracter(lexico->entrada);
    while(c != EOF && isspace((unsigned char)c)){
        c = sigCaracter(lexico->entrada);
    }
    if(c != EOF){
        retrocederCaracter(lexico->entrada);
    }
}

// Lee un identificador completo. Luego ya se decide si era ID o reservada.
static TokenLexico reconocerIdentificadorOPalabraReservada(AnalizadorLexico *lexico){
    int c = sigCaracter(lexico->entrada);

    while(c != EOF && (isalnum((unsigned char)c) || c == '_')){
        c = sigCaracter(lexico->entrada);
    }

    return cerrarYResolverEnTS(lexico, c);
}

// Continua leyendo la parte exponencial de un real.
static TokenLexico reconocerExponente(AnalizadorLexico *lexico){
    int c = sigCaracter(lexico->entrada);
    int tieneDigito = 0;

    if(c == '+' || c == '-'){
        c = sigCaracter(lexico->entrada);
    }

    while(c != EOF && (isdigit((unsigned char)c) || c == '_')){
        if(isdigit((unsigned char)c)){
            tieneDigito = 1;
        }
        c = sigCaracter(lexico->entrada);
    }

    if(!tieneDigito){
        if(c != EOF){
            retrocederCaracter(lexico->entrada);
        }
        reportarError(ERROR_LEXICO, "Numero mal formado: exponente sin digitos");
        limpiarTokenActual(lexico);
        return sigCompLexico(lexico);
    }

    if(c != EOF){
        retrocederCaracter(lexico->entrada);
    }

    free(lexico->lexemaActual);
    // Al final volvemos a pedir el lexema entero, no solo el exponente.
    lexico->lexemaActual = obtenerLexema(lexico->entrada);
    if(!lexico->lexemaActual){
        limpiarTokenActual(lexico);
        return sigCompLexico(lexico);
    }

    return crearToken(FLOTANTE, lexico->lexemaActual);
}

// Lee la parte decimal despues del punto. Si aparece exponente, salta a ese caso.
static TokenLexico reconocerFraccion(AnalizadorLexico *lexico){
    int c = sigCaracter(lexico->entrada);

    while(c != EOF && (isdigit((unsigned char)c) || c == '_')){
        c = sigCaracter(lexico->entrada);
    }

    if(c == 'e' || c == 'E'){
        return reconocerExponente(lexico);
    }

    if(c != EOF){
        retrocederCaracter(lexico->entrada);
    }

    free(lexico->lexemaActual);
    lexico->lexemaActual = obtenerLexema(lexico->entrada);
    if(!lexico->lexemaActual){
        limpiarTokenActual(lexico);
        return sigCompLexico(lexico);
    }

    return crearToken(FLOTANTE, lexico->lexemaActual);
}

// Lee un literal binario de los que empiezan por 0b o 0B.
static TokenLexico reconocerBinario(AnalizadorLexico *lexico){
    int c = sigCaracter(lexico->entrada);
    int tieneDigito = 0;

    while(c != EOF && (c == '0' || c == '1')){
        tieneDigito = 1;
        c = sigCaracter(lexico->entrada);
    }

    if(!tieneDigito){
        if(c != EOF){
            retrocederCaracter(lexico->entrada);
        }
        reportarError(ERROR_LEXICO, "Numero binario mal formado");
        limpiarTokenActual(lexico);
        return sigCompLexico(lexico);
    }

    if(c != EOF){
        retrocederCaracter(lexico->entrada);
    }

    free(lexico->lexemaActual);
    lexico->lexemaActual = obtenerLexema(lexico->entrada);
    if(!lexico->lexemaActual){
        limpiarTokenActual(lexico);
        return sigCompLexico(lexico);
    }
    return crearToken(ENTERO, lexico->lexemaActual);
}

// Caso normal para numeros decimales: entero, fraccion o exponente.
static TokenLexico reconocerDecimal(AnalizadorLexico *lexico){
    int c = sigCaracter(lexico->entrada);

    while(c != EOF && (isdigit((unsigned char)c) || c == '_')){
        c = sigCaracter(lexico->entrada);
    }

    if(c == '.'){
        return reconocerFraccion(lexico);
    }

    if(c == 'e' || c == 'E'){
        return reconocerExponente(lexico);
    }

    if(c != EOF){
        retrocederCaracter(lexico->entrada);
    }

    free(lexico->lexemaActual);
    lexico->lexemaActual = obtenerLexema(lexico->entrada);
    if(!lexico->lexemaActual){
        limpiarTokenActual(lexico);
        return sigCompLexico(lexico);
    }
    return crearToken(ENTERO, lexico->lexemaActual);
}

// El cero se mira aparte porque puede abrir un binario o una fraccion.
static TokenLexico reconocerCero(AnalizadorLexico *lexico){
    int c = sigCaracter(lexico->entrada);

    if(c == 'b' || c == 'B'){
        return reconocerBinario(lexico);
    }

    if(c == '.'){
        return reconocerFraccion(lexico);
    }

    if(c == 'e' || c == 'E'){
        return reconocerExponente(lexico);
    }

    if(c != EOF){
        retrocederCaracter(lexico->entrada);
    }

    free(lexico->lexemaActual);
    lexico->lexemaActual = obtenerLexema(lexico->entrada);
    if(!lexico->lexemaActual){
        limpiarTokenActual(lexico);
        return sigCompLexico(lexico);
    }
    return crearToken(ENTERO, lexico->lexemaActual);
}

// Lee strings entre comillas dobles. Si hay una barra invertida, la siguiente
// comilla no cierra la cadena y se sigue tirando.
static TokenLexico reconocerString(AnalizadorLexico *lexico){
    int c = sigCaracter(lexico->entrada);
    int escapado = 0;
    int cerrado = 0;

    while(c != EOF){
        if(escapado){
            escapado = 0;
        }else if(c == '\\'){
            escapado = 1;
        }else if(c == '"'){
            cerrado = 1;
            break;
        }
        c = sigCaracter(lexico->entrada);
    }

    if(!cerrado){
        reportarError(ERROR_LEXICO, "String sin comilla de cierre");
        limpiarTokenActual(lexico);
        return sigCompLexico(lexico);
    }

    free(lexico->lexemaActual);
    lexico->lexemaActual = obtenerLexema(lexico->entrada);
    if(!lexico->lexemaActual){
        limpiarTokenActual(lexico);
        return sigCompLexico(lexico);
    }
    
    return crearToken(STRING, lexico->lexemaActual);
}


// Auotmata de comentarios, aunque tambien puede ser operador 
static TokenLexico reconocerComentarioOOperador(AnalizadorLexico *lexico){
    int c = sigCaracter(lexico->entrada);

    if(c == '/'){
        c = sigCaracter(lexico->entrada);
        while(c != EOF && c != '\n'){
            c = sigCaracter(lexico->entrada);
        }
        if(c == '\n'){
            // El salto de linea no forma parte del comentario que queremos guardar.
            retrocederCaracter(lexico->entrada);
        }
        free(lexico->lexemaActual);
        lexico->lexemaActual = obtenerLexema(lexico->entrada);
        if(!lexico->lexemaActual){
            limpiarTokenActual(lexico);
            return sigCompLexico(lexico);
        }
        return crearToken(COMENTARIO, lexico->lexemaActual);
    }

    if(c == '*'){
        int anterior = 0;
        int cerrado = 0;
        c = sigCaracter(lexico->entrada);
        while(c != EOF){
            if(anterior == '*' && c == '/'){
                cerrado = 1;
                break;
            }
            anterior = c;
            c = sigCaracter(lexico->entrada);
        }
        if(!cerrado){
            reportarError(ERROR_LEXICO, "Comentario /* sin cierre */");
            limpiarTokenActual(lexico);
            return sigCompLexico(lexico);
        }
        free(lexico->lexemaActual);
        lexico->lexemaActual = obtenerLexema(lexico->entrada);
        if(!lexico->lexemaActual){
            limpiarTokenActual(lexico);
            return sigCompLexico(lexico);
        }
        return crearToken(COMENTARIO, lexico->lexemaActual);
    }

    if(c == '+'){
        int profundidad = 1;
        int anterior = 0;

        c = sigCaracter(lexico->entrada);
        while(c != EOF && profundidad > 0){
            if(anterior == '/' && c == '+'){
                profundidad++;
            }else if(anterior == '+' && c == '/'){
                profundidad--;
            }
            anterior = c;
            if(profundidad > 0){
                c = sigCaracter(lexico->entrada);
            }
        }

        if(profundidad > 0){
            reportarError(ERROR_LEXICO, "Comentario /+ sin cierre +/");
            limpiarTokenActual(lexico);
            return sigCompLexico(lexico);
        }

        free(lexico->lexemaActual);
        lexico->lexemaActual = obtenerLexema(lexico->entrada);
        if(!lexico->lexemaActual){
            limpiarTokenActual(lexico);
            return sigCompLexico(lexico);
        }
        return crearToken(COMENTARIO, lexico->lexemaActual);
    }

    if(c != EOF){
        retrocederCaracter(lexico->entrada);
    }
    cancelarLexemaActual(lexico->entrada);
    return crearToken('/', "/");
}

// Aqui se resuelven los simbolos sueltos y los operadores compuestos.
static TokenLexico reconocerOperadorOSimbolo(AnalizadorLexico *lexico, int c){
    int siguiente = sigCaracter(lexico->entrada);

    switch(c){
    case '=':
        if(siguiente == '='){
            cancelarLexemaActual(lexico->entrada);
            return crearToken(OP_IGUAL_IGUAL, "==");
        }
        break;
    case '+':
        if(siguiente == '+'){
            cancelarLexemaActual(lexico->entrada);
            return crearToken(OP_MAS_MAS, "++");
        }
        if(siguiente == '='){
            cancelarLexemaActual(lexico->entrada);
            return crearToken(OP_MAS_IGUAL, "+=");
        }
        break;
    case '-':
        break;
    default:
        break;
    }

    if(siguiente != EOF){
        retrocederCaracter(lexico->entrada);
    }

    free(lexico->lexemaActual);
    lexico->lexemaActual = obtenerLexema(lexico->entrada);
    if(!lexico->lexemaActual){
        limpiarTokenActual(lexico);
        return sigCompLexico(lexico);
    }

    return crearToken((unsigned char)c, lexico->lexemaActual);
}


// Punto de entrada del lexico. Mira el primer caracter y decide a que automata llamar 
TokenLexico sigCompLexico(AnalizadorLexico *lexico){
    int c;
    TokenLexico token;

    if(!lexico || !lexico->entrada || !lexico->ts){
        return crearToken(EOF, "EOF");
    }

    ignorarBlancos(lexico);
    c = sigCaracter(lexico->entrada);

    if(c == EOF){
        free(lexico->lexemaActual);
        lexico->lexemaActual = NULL;
        cancelarLexemaActual(lexico->entrada);
        return crearToken(EOF, "EOF");
    }

    // le digo al sistema de entrada que empieza aquí el lexema para que ajuste inicio 
    marcarInicioLexema(lexico->entrada, c);

    if(isalpha((unsigned char)c) || c == '_'){
        return reconocerIdentificadorOPalabraReservada(lexico);
    }

    if((unsigned char)c == '0'){
        return reconocerCero(lexico);
    }

    if(isdigit((unsigned char)c)){
        return reconocerDecimal(lexico);
    }

    if((unsigned char)c =='"'){
        return reconocerString(lexico);
    }

    if((unsigned char)c == '/'){
        token = reconocerComentarioOOperador(lexico);
        if(token.identificador == COMENTARIO){
            // Los comentarios no salen al sintactico, se leen y se tiran.
            return sigCompLexico(lexico);
        }
        return token;
    }

    return reconocerOperadorOSimbolo(lexico, c);
}
