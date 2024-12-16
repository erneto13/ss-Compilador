#ifndef DEFINICIONLEXICO_H
#define DEFINICIONLEXICO_H
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

// Función para formeatear un token
void limpiarToken(struct Token token) {
    token.valor = 0;

    // Borra el contenido de lexema y nombre
    memset(token.lexema, 0, sizeof(token.lexema));
    memset(token.nombre, 0, sizeof(token.nombre));
    token.tipo = 0;
    token.numCol = 0;
    token.numLin = 0;
}


void insertar(struct Token token) {
    // Crea un nuevo nodo para el token
    struct nodo *nuevo = malloc(sizeof(struct nodo));
    nuevo->info = token;
    nuevo->izq = NULL;
    nuevo->der = NULL;

    // Si la lista está vacía, establece como raíz
    /*
     * Cuando insertamos la primera "bolita" (nodo):
     * - Si no hay ninguna bolita en el tren (lista vacía),
     *   esta bolita se convierte en la primera (raíz)
     * - También se establece como la bolita actual
     *
     * Cuando insertamos bolitas adicionales:
     * 1. El lado izquierdo de la nueva bolita apunta a la bolita anterior
     * 2. El lado derecho de la bolita anterior apunta a la nueva bolita
     * 3. La nueva bolita se convierte en la bolita actual (última del tren)
     *
     * Es como agregar vagones a un tren:
     * - El primer vagón es la locomotora (raíz)
     * - Cada nuevo vagón se conecta al vagón anterior
     * - El último vagón agregado es el punto actual de referencia
     */
    if (raiz == NULL) {
        raiz = nuevo;
        act = nuevo;
    } else {
        nuevo->izq = act;
        act->der = nuevo;
        act = nuevo;
    }
}

// Imprime la lista de Tokens con los atributos de cada nodo
void imprimirLista(struct nodo *reco) {
    if (reco != NULL) {
        printf("Nombre: %s\t\t", reco->info.nombre);
        printf("Tipo: %d\t", reco->info.tipo);
        switch (reco->info.tipo) {
            case 0: {
                printf("Tipo: PALRES\t");
                break;
            }
            case 1: {
                printf("Tipo: ID\t");
                break;
            }
            case 2: {
                printf("Tipo: NUM\t");
                break;
            }
            case 3: {
                printf("Tipo: SIM\t");
                break;
            }
            case 4: {
                printf("Tipo: TD\t");
                break;
            }
            case 5: {
                printf("Tipo: CADENA\t");
                break;
            }
            case 6: {
                printf("Tipo: ERROR\t");
                break;
            }
        }
        printf("Lexema: %s\t\t", reco->info.lexema);
        printf("Valor: %.2f ", reco->info.valor);
        printf("Línea: %d Columna: %d\n", reco->info.numLin, reco->info.numCol);
        imprimirLista(reco->der);
    }
}

// Encuentra el total de caracteres en el archivo moviendo el
// puntero que lo lee, al final reinicia el puntero a su posición original.
int tamArchivo(FILE *f) {
    size_t pos = ftell(f);
    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    fseek(f, pos, SEEK_SET);
    return length;
}

/*
 * PARTE 2- AUTOMATAS
 * - esLimitador: Verifica si un caracter es un limitador
 * - esTerminador: Verifica si un caracter es un terminador
 * - tipoDato: Verifica si un string es un tipo de dato
 * - operador: Verifica si un caracter es un operador
 * - esNumero: Verifica si un string es un número
 * - palReservada: Verifica si un string es una palabra reservada
 * - esBool: Verifica si un string es un booleano
 * - esIdentificador: Verifica si un string es un identificador
 * - esCadena: Verifica si un string es una cadena
 */

bool esLimitador(char c) {
    if (c == ' ' || c == ';' || c == '[' || c == ']' || c == '(' || c == ')' || c == '\n' || c == EOF || c == '\t' ||
        c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '=' || c == '|' || c == '&' ||
        c == '{' || c == '}') {
        return true;
    }
    return false;
}

bool esTerminador(char c, struct Token *token) {
    if (c == ';' || c == '[' || c == ']' || c == '(' || c == ')' || c == '"' || c == '|' || c == '&' || c == '{' || c ==
        '}') {
        memset(token->lexema, 0, sizeof(token->lexema));
        token->lexema[0] = c;
        token->tipo = TK_SIMBOLO;
        token->valor = 0;
        switch (c) {
            case ';': {
                strcpy(token->nombre, "PUNTO Y COMA");
                break;
            }
            case '[': {
                strcpy(token->nombre, "CORCHETE IZQ");
                break;
            }
            case ']': {
                strcpy(token->nombre, "CORCHETE DER");
                break;
            }
            case '(': {
                strcpy(token->nombre, "PARENTESIS IZQ");
                break;
            }
            case ')': {
                strcpy(token->nombre, "PARENTESIS DER");
                break;
            }
            case '"': {
                strcpy(token->nombre, "COMILLA DOBLE");
                break;
            }
            case '|': {
                strcpy(token->nombre, "OR");
                break;
            }
            case '&': {
                strcpy(token->nombre, "AND");
                break;
            }
            case '{': {
                strcpy(token->nombre, "LLAVE IZQ");
                break;
            }
            case '}': {
                strcpy(token->nombre, "LLAVE DER");
                break;
            }
            default:
                break;
        }
        return true;
    }
    return false;
}

bool tipoDato(const char *str, struct Token *token) {
    const int numTiposDeDatos = sizeof(tiposDeDatos) / sizeof(tiposDeDatos[0]);

    for (int i = 0; i < numTiposDeDatos; i++) {
        if (strcmp(str, tiposDeDatos[i]) == 0) {
            // Configura propiedades del token para tipo de dato
            strcpy(token->lexema, str);
            token->tipo = TK_TIPODATO;
            token->valor = 0;
            strcpy(token->nombre, "TIPO DATO");
            return true;
        }
    }
    return false;
}

bool operador(char c, struct Token *token) {
    if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '>' || c == '<') {
        memset(token->lexema, 0, sizeof(token->lexema));
        token->lexema[0] = c;
        token->valor = 0;
        token->tipo = TK_SIMBOLO;
        switch (c) {
            case '+': {
                strcpy(token->nombre, "MAS");
                break;
            }
            case '-': {
                strcpy(token->nombre, "MENOR");
                break;
            }
            case '*': {
                strcpy(token->nombre, "POR");
                break;
            }
            case '/': {
                strcpy(token->nombre, "ENTRE");
                break;
            }
            case '=': {
                strcpy(token->nombre, "IGUAL");
                break;
            }
            case '>': {
                strcpy(token->nombre, "MAYOR");
                break;
            }
            case '<': {
                strcpy(token->nombre, "MENOR");
                break;
            }
            default:
                break;
        }
        return true;
    }
    return false;
}

bool esNumero(char *str, struct Token *token) {
    if (str[0] >= '0' && str[0] <= '9') {
        int punto = 0;
        int z = 1;
        while (z <= strlen(str) - 1) {
            if (str[z] == '.') {
                punto++;
            }
            if (!(str[z] == '.' || (str[z] >= '0' && str[z] <= '9')) && punto == 0) {
                return false;
            }
            z++;
        }
        strcpy(token->lexema, str);
        token->tipo = TK_NUMERO;
        strcpy(token->nombre, "CONSTANTE");
        token->valor = atof(str);
        return true;
    }
    return false;
}

bool palReservada(char *str, struct Token *token) {
    const int numPalabrasReservadas = sizeof(palabrasReservadas) / sizeof(palabrasReservadas[0]);

    for (int i = 0; i < numPalabrasReservadas; i++) {
        if (strcmp(str, palabrasReservadas[i]) == 0) {
            // Configura propiedades del token de palabra reservada
            strcpy(token->lexema, str);
            token->tipo = TK_PALABRARESERVADA;
            strcpy(token->nombre, "INSTRUCCION");
            token->valor = 0;
            return true;
        }
    }
    return false;
}

bool esBool(char *str, struct Token *token) {
    if (strcmp(str, "verdadero") == 0 || strcmp(str, "falso") == 0) {
        strcpy(token->lexema, str);
        token->tipo = TK_BOOLEANO;
        strcpy(token->nombre, str);
        token->valor = 0;
        return true;
    }
    return false;
}

bool esIdentificador(const char *str, struct Token *token) {
    if (str[0] >= 'a' && str[0] <= 'z') {
        int z = 1;
        bool aux;
        while (z <= strlen(str) - 1) {
            char c = str[z];
            if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_')) {
                return false;
            }
            z++;
        }
        strcpy(token->lexema, str);
        token->tipo = TK_IDENTIFICADOR;
        strcpy(token->nombre, "ID");
        token->valor = 0;
        return true;
    }
    return false;
}

bool esCadena(char *str, struct Token *token) {
    char c = str[0];
    char z = str[strlen(str) - 1];
    if (str[0] == '"' && (str[strlen(str) - 1] == '"' || str[strlen(str) - 1 == ' '])) {
        strcpy(token->lexema, str);
        token->tipo = TK_CADENA;
        strcpy(token->nombre, "CADENA");
        token->valor = 0;
        return true;
    } else {
        return false;
    }
}

void leerTokens(const char *prog) {
    int lines = 1;
    int columna = 0;
    char aux[255] = {0};
    int x = 0;
    int y = 0;
    struct Token token;
    bool cadena = false;
    int cadenaAux = 0;
    while (prog[x] != EOF) {
        if (prog[x] == '\n') {
            lines++;
            columna = 0;
        }
        char c = prog[x];
        if (cadena == true && cadenaAux != 2) {
            if (prog[x] == '"') {
                cadena = true;
                cadenaAux++;
            }
            aux[y] = prog[x];
            y++;
        } else if (cadenaAux == 2) {
            if (esCadena(aux, &token)) {
                cadena = false;
                cadenaAux = 0;
                token.numCol = columna;
                token.numLin = lines;
                insertar(token);
                limpiarToken(token);
            } else if (aux[0] == '"') {
                token.tipo = TK_ERROR;
                strcpy(token.nombre, "ERROR COMILLA");
                token.valor = 0;
                strcpy(token.lexema, "ERROR COMILLA");
                token.numCol = columna;
                token.numLin = lines;
                insertar(token);
                limpiarToken(token);
            }
            memset(aux, 0, sizeof(aux));
            y = 0;
            continue;
        } else {
            if (esLimitador(prog[x])) {
                if (y != 0) {
                    if (tipoDato(aux, &token)) {
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                    } else if (palReservada(aux, &token)) {
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                    } else if (esBool(aux, &token)) {
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                    } else if (esIdentificador(aux, &token)) {
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                    } else if (esNumero(aux, &token)) {
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                    } else if (aux[0] == '"') {
                        token.tipo = TK_ERROR;
                        strcpy(token.nombre, "ERROR COMILLA");
                        token.valor = 0;
                        strcpy(token.lexema, "ERROR COMILLA");
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                    } else {
                        token.tipo = TK_ERROR;
                        strcpy(token.nombre, "ERROR");
                        token.valor = 0;
                        strcpy(token.lexema, "ERROR");
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                    }
                    memset(aux, 0, sizeof(aux));
                    y = 0;
                    continue;
                } else {
                    if (operador(prog[x], &token)) {
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                    } else if (esTerminador(prog[x], &token)) {
                        if (prog[x] == '\n') {
                            lines++;
                            columna = 0;
                        } else if (prog[x] == EOF) {
                        } else {
                        }
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                    }
                }
            } else {
                if (prog[x] == '"') {
                    cadena = true;
                    cadenaAux++;
                }
                aux[y] = prog[x];
                y++;
            }
        }
        columna++;
        x++;
    }
    if (cadena == true) {
        token.tipo = TK_ERROR;
        strcpy(token.nombre, "ERROR COMILLA");
        token.valor = 0;
        strcpy(token.lexema, "ERROR COMILLA");
        token.numCol = columna;
        token.numLin = lines;
        insertar(token);
        limpiarToken(token);
    }
}

#endif //DEFINICIONLEXICO_H
