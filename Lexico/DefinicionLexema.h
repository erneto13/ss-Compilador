#ifndef DEFINICIONLEXEMA_H
#define DEFINICIONLEXEMA_H

#define MAX_LONGITUD 100
#include <stdio.h>

// ENUM que define los diferentes
// tipos de tokens reconocidos
typedef enum tipoToken {
    TK_PALABRARESERVADA,
    TK_IDENTIFICADOR,
    TK_NUMERO,
    TK_SIMBOLO,
    TK_TIPODATO,
    TK_CADENA,
    TK_ERROR,
    TK_BOOLEANO
} TipoToken;

// Estructura que representa un
// token en el análisis léxico
struct Token {
    char nombre[255];
    enum tipoToken tipo;
    char lexema[255];
    float valor;
    int numLin;
    int numCol;
};

// Estructura de un nodo para un
// árbol binario de símbolos
struct nodo {
    struct Token info;
    struct nodo *izq;
    struct nodo *der;
};

// Array con los tipos de datos reconocidos en el lenguaje
char *tiposDeDatos[] = {
    "entero", "real", "cadena", "caracter", "booleano"
};

// Array con las palabras reservadas del lenguaje
char *palabrasReservadas[] = {
    "declaracion", "principal", "regresa", "Mostrar", "Leer",
    "si", "entonces", "para", "mientras", "sino"
};

struct nodo *raiz = NULL;
struct nodo *act = NULL;

#endif //DEFINICIONLEXEMA_H