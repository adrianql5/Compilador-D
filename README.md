# Practica 1

## Que hay que tener instalado

Para compilar y ejecutar el programa hace falta esto:

- `gcc`
- `make`


## Ficheros importantes

- `main.c`: arranca el programa
- `regression.d`: fichero de entrada que se analiza
- `Makefile`: compila todo el proyecto

## Como compilar

Desde la carpeta del proyecto:

```bash
make
```

Eso genera el ejecutable:

```bash
./compilador
```

## Como ejecutar

El programa no recibe argumentos.

Siempre:

- usa `regression.d` como fichero de entrada
- usa tamaño de bloque `2048`

Para ejecutarlo:

```bash
./compilador
```

## Que hace al ejecutarse

El programa:

1. crea la tabla de simbolos
2. inicializa el sistema de entrada
3. inicializa el analizador lexico
4. recorre `regression.d`
5. va imprimiendo tokens por pantalla
6. al final libera la memoria usada

Tambien imprime la tabla de simbolos antes y despues del analisis.

## Si da error al compilar

Comprueba estas cosas:

- que `gcc` este instalado
- que `make` este instalado
- que estes situado dentro de la carpeta del proyecto
- que exista el fichero `regression.d`

## Limpiar el ejecutable

Si quieres borrar el binario compilado:

```bash
make clean
```
# Compilador-D
