#ifndef DEFINICIONSINTACTICO_H
#define DEFINICIONSINTACTICO_H

struct nodo *sintaxisExp(struct nodo *reco, int edo) {
    if (edo == 1) {
        // Estado inicial: esperando IDENTIFICADOR, NÚMERO o '('.
        if (reco->info.tipo == TK_IDENTIFICADOR || reco->info.tipo == TK_NUMERO) {
            edo = 2; // Cambia al estado donde esperamos operadores o fin de la expresión.
            sintaxisExp(reco->der, edo);
        } else if (strcmp(reco->info.lexema, "(") == 0) {
            edo = 3; // Abre un paréntesis, así que pasamos a procesar el contenido dentro.
            sintaxisExp(reco->der, edo);
        } else {
            // Error: algo que no era esperado al inicio de la expresión.
            printf("ERROR DE SINTAXIS :: Se esperaba un IDENTIFICADOR, NUMERO o un PARENTESIS en línea %d columna %d",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 2) {
        // Estado esperando un operador (+, -, *, /) o el fin de una expresión.
        if (strcmp(reco->info.lexema, "+") == 0 || strcmp(reco->info.lexema, "-") == 0 ||
            strcmp(reco->info.lexema, "*") == 0 || strcmp(reco->info.lexema, "/") == 0) {
            edo = 1; // Operador encontrado, volvemos a esperar otra expresión.
            sintaxisExp(reco->der, edo);
        } else if (strcmp(reco->info.lexema, ")") == 0) {
            return reco; // Paréntesis de cierre encontrado, terminamos aquí.
        } else if (strcmp(reco->info.lexema, ";") == 0) {
            edo = 5; // Punto y coma: probablemente es el fin de la instrucción.
            sintaxisExp(reco, edo);
        } else {
            // Error: no era operador ni paréntesis ni punto y coma.
            printf("ERROR DE SINTAXIS :: Se esperaba un SIMBOLO OPERADOR en línea %d columna %d",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 3) {
        // Estado dentro de un paréntesis: volvemos a procesar desde el estado inicial.
        edo = 1;
        reco = sintaxisExp(reco, edo);
        if (strcmp(reco->info.lexema, ")") == 0) {
            edo = 2; // Cierre del paréntesis, volvemos al estado después de un valor válido.
            sintaxisExp(reco->der, edo);
        } else {
            // Error: no encontramos un paréntesis de cierre.
            printf("ERROR DE SINTAXIS :: Se esperaba un PARENTESIS DER \")\" en la línea %d columna %d",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 5) {
        // Estado final: asegurar que hay un punto y coma correctamente.
        if (strcasecmp(reco->info.lexema, ";") != 0) {
            // Error: no encontramos el punto y coma.
            printf("ERROR DE SINTAXIS :: Se esperaba el símbolo PUNTO Y COMA ( ; ) en línea %d columna %d",
                   reco->info.numLin, reco->info.numCol);
        } else {
            return reco; // Todo bien, terminamos.
        }
    }
}


struct nodo *sintaxisVariable(struct nodo *reco, int edo) {
    if (edo == 1) {
        // Estado inicial: esperando un tipo de dato (entero, cadena, real, etc).
        if (reco->info.tipo == TK_TIPODATO) {
            edo = 2; // Tipo de dato encontrado, pasamos al siguiente estado.
            sintaxisVariable(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba un TIPO DE DATO en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 2) {
        // Estado esperando un identificador (nombre de la variable).
        if (reco->info.tipo == TK_IDENTIFICADOR) {
            edo = 3; // Identificador válido, avanzamos.
            sintaxisVariable(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba un IDENTIFICADOR en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 3) {
        // Estado esperando el símbolo '=' para la asignación.
        if (strcasecmp(reco->info.lexema, "=") == 0) {
            edo = 4; // Símbolo de igualdad encontrado, pasamos a esperar un valor.
            sintaxisVariable(reco->der, edo);
        } else {
            // Error: no se encuentra el símbolo '=' después del identificador.
            printf("ERROR DE SINTAXIS :: Se esperaba el SIMBOLO DE IGUALDAD ( = ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 4) {
        // Estado esperando un valor válido (número, cadena, booleano, etc.).
        if ((reco->info.tipo == TK_CADENA) || (reco->info.tipo == TK_NUMERO) || (reco->info.tipo == TK_BOOLEANO)) {
            edo = 5; // Valor válido encontrado, pasamos al estado final.
            sintaxisVariable(reco->der, edo);
        } else if (reco->info.tipo == TK_IDENTIFICADOR) {
            printf(
                "ERROR DE SINTAXIS :: Una variable sólo se puede inicializar con un valor constante, en la línea %d columna %d.",
                reco->info.numLin, reco->info.numCol);
        } else {
            // Error: no se asigna un valor válido a la variable.
            printf("ERROR DE SINTAXIS :: Debe asignarse un valor a la variable en la línea %d columna %d.",
                   reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 5) {
        // Estado final: verificando que la declaración termine con un ';'.
        if (strcasecmp(reco->info.lexema, ";") != 0) {
            printf("ERROR DE SINTAXIS :: Se esperaba el símbolo PUNTO Y COMA ( ; ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der); // Todo correcto, regresamos el nodo.
        }
    }
}


struct nodo *sintaxisConstante(struct nodo *reco, int edo) {
    if (edo == 1) {
        if (strcasecmp(reco->info.lexema, "CONSTANTE") == 0) {
            edo = 2;
            sintaxisConstante(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba la palabra CONSTANTE en la línea %d columna %d.",
                   reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 2) {
        if (reco->info.tipo == TK_TIPODATO) {
            edo = 3;
            sintaxisVariable(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba un TIPO DE DATO en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 3) {
        if (reco->info.tipo == TK_IDENTIFICADOR) {
            edo = 4;
            sintaxisVariable(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba un IDENTIFICADOR en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 4) {
        if (strcasecmp(reco->info.lexema, "=") == 0) {
            edo = 5;
            sintaxisVariable(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el SIMBOLO DE IGUALDAD ( = ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 5) {
        if (reco->info.tipo == TK_NUMERO) {
            edo = 6;
            sintaxisVariable(reco->der, edo);
        } else if (reco->info.tipo == TK_CADENA) {
            edo = 6;
            sintaxisVariable(reco->der, edo);
        } else if (reco->info.tipo == TK_BOOLEANO) {
            edo = 6;
            sintaxisVariable(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Debe asignarse un valor a la variable en la línea %d columna %d.",
                   reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 6) {
        if (strcasecmp(reco->info.lexema, ";") != 0) {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo PUNTO Y COMA ( ; ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

struct nodo *sintaxisLeer(struct nodo *reco, int edo) {
    if (edo == 1) {
        if (strcasecmp(reco->info.lexema, "Leer") == 0) {
            edo = 2;
            sintaxisLeer(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba la palabra 'Leer' en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 2) {
        if (strcasecmp(reco->info.lexema, "(") == 0) {
            edo = 3;
            sintaxisLeer(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo PARENTESIS APERTURA ( ( )  en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 3) {
        if (reco->info.tipo == 1) {
            edo = 4;
            sintaxisLeer(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba un IDENTIFICADOR LEER en la línea %d columna %d.",
                   reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 4) {
        if (strcasecmp(reco->info.lexema, ")") == 0) {
            edo = 5;
            sintaxisLeer(reco->der, edo);
        } else {
            printf(
                "ERROR DE SINTAXIS :: Se esperaba el simbolo PARENTESIS DE CIERRE ( ) ) Leer  en la línea %d columna %d.",
                reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 5) {
        if (strcasecmp(reco->info.lexema, ";") != 0) {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo PUNTO Y COMA ( ; ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

struct nodo *sintaxisEscribir(struct nodo *reco, int edo) {
    if (edo == 1) {
        if (strcasecmp(reco->info.lexema, "Mostrar") == 0) {
            edo = 2;
            sintaxisEscribir(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba la palabra 'Mostrar' en la línea %d columna %d.",
                   reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 2) {
        if (strcasecmp(reco->info.lexema, "(") == 0) {
            edo = 3;
            sintaxisEscribir(reco->der, edo);
        } else {
            printf(
                "ERROR DE SINTAXIS :: Se esperaba el simbolo PARENTESIS DE APERTURA ( ( )  en la línea %d columna %d.",
                reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 3) {
        if ((reco->info.tipo == 1) || (reco->info.tipo == 5) || (strcasecmp(reco->info.lexema, "+") == 0)) {
            edo = 3;
            sintaxisEscribir(reco->der, edo);
        } else if (!((reco->info.tipo == 1) || (reco->info.tipo == 5) || (strcasecmp(reco->info.lexema, "+") == 0))) {
            edo = 4;
            sintaxisEscribir(reco, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba una CADENA o un IDENTIFICADOR en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 4) {
        if (strcasecmp(reco->info.lexema, ")") == 0) {
            edo = 5;
            sintaxisEscribir(reco->der, edo);
        } else {
            printf(
                "ERROR DE SINTAXIS :: Se esperaba el simbolo PARENTESIS DE CIERRE ( ) ) Mostrar en la línea %d columna %d.",
                reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 5) {
        if (strcasecmp(reco->info.lexema, ";") != 0) {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo PUNTO Y COMA ( ; ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

struct nodo *sintaxisAsignar(struct nodo *reco, int edo) {
    if (edo == 1) {
        if (reco->info.tipo == TK_IDENTIFICADOR) {
            edo = 2;
            sintaxisAsignar(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba un IDENTIFICADOR en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 2) {
        if (strcasecmp(reco->info.lexema, "=") == 0) {
            edo = 3;
            sintaxisAsignar(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo de ASIGNACION ( = ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 3) {
        if (reco->info.tipo == TK_IDENTIFICADOR || strcasecmp(reco->info.lexema, "(") == 0 || (
                (reco->info.tipo == TK_NUMERO || reco->info.tipo == TK_CADENA) && (
                    strcmp(reco->der->info.lexema, "+") == 0 || strcmp(reco->der->info.lexema, "-") == 0 ||
                    strcmp(reco->der->info.lexema, "*") == 0 || strcmp(reco->der->info.lexema, "/") == 0 || strcmp(
                        reco->der->info.lexema, "(") == 0))) {
            edo = 4;
            struct nodo *recoAux = sintaxisExp(reco, 1);
            sintaxisAsignar(recoAux, edo);
        } else if ((reco->info.tipo == TK_CADENA) || (reco->info.tipo == TK_NUMERO) || (
                       reco->info.tipo == TK_BOOLEANO)) {
            edo = 4;
            sintaxisAsignar(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba una ASIGNACION en la linea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 4) {
        if (strcasecmp(reco->info.lexema, ";") != 0) {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo PUNTO Y COMA ( ; ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

struct nodo *sintaxisIf(struct nodo *reco, int edo);

struct nodo *sintaxisMientras(struct nodo *reco, int edo);

struct nodo *sintaxisPara(struct nodo *reco, int edo);

struct nodo *sintaxisSino(struct nodo *reco, int edo) {
    if (edo == 1) {
        if (strcasecmp(reco->info.lexema, "sino") == 0) {
            edo = 2;
            sintaxisSino(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba la palabra ( sino ) en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 2) {
        if (strcasecmp(reco->info.lexema, "{") == 0) {
            edo = 3;
            sintaxisSino(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo LLAVE DE APERTURA ( { ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 3) {
        if (strcasecmp(reco->info.lexema, "si") == 0) {
            edo = 3;
            struct nodo *recoAux = sintaxisIf(reco, 1);
            sintaxisSino(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "Leer") == 0) {
            edo = 3;
            struct nodo *recoAux = sintaxisLeer(reco, 1);
            sintaxisSino(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "Mostrar") == 0) {
            edo = 3;
            struct nodo *recoAux = sintaxisEscribir(reco, 1);
            sintaxisSino(recoAux, edo);
        } else if (reco->info.tipo == TK_IDENTIFICADOR) {
            edo = 3;
            struct nodo *recoAux = sintaxisAsignar(reco, 1);
            sintaxisSino(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "mientras") == 0) {
            edo = 3;
            struct nodo *recoAux = sintaxisMientras(reco, 1);
            sintaxisSino(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "para") == 0) {
            edo = 3;
            struct nodo *recoAux = sintaxisPara(reco, 1);
            sintaxisSino(recoAux, edo);
        } else {
            edo = 4;
            sintaxisSino(reco, edo);
        }
    } else if (edo == 4) {
        if (strcasecmp(reco->info.lexema, "}") == 0) {
            return (reco->der);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo LLAVE DE CIERRE ( } ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    }
}

struct nodo *sintaxisIf(struct nodo *reco, int edo) {
    if (edo == 1) {
        if (strcasecmp(reco->info.lexema, "si") == 0) {
            edo = 2;
            sintaxisIf(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba la palabra ( si ) en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 2) {
        if (strcasecmp(reco->info.lexema, "(") == 0) {
            edo = 3;
            sintaxisIf(reco->der, edo);
        } else {
            printf(
                "ERROR DE SINTAXIS :: Se esperaba el simbolo PARENTESIS DE APERTURA ( ( ) en la línea %d columna %d.",
                reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 3) {
        if ((strcasecmp(reco->info.lexema, "verdadero") == 0) || (strcasecmp(reco->info.lexema, "falso") == 0)) {
            edo = 4;
            sintaxisIf(reco->der, edo);
        } else {
            printf("Se esperaba un valor VERDADERO o FALSO en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 4) {
        if (strcasecmp(reco->info.lexema, ")") == 0) {
            edo = 5;
            sintaxisIf(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo PARENTESIS DE CIERRE ( ) ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 5) {
        if (strcasecmp(reco->info.lexema, "{") == 0) {
            edo = 6;
            sintaxisIf(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo LLAVE DE APERTURA ( { ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 6) {
        if (strcasecmp(reco->info.lexema, "si") == 0) {
            edo = 6;
            struct nodo *recoAux = sintaxisIf(reco, 1);
            sintaxisIf(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "Leer") == 0) {
            edo = 6;
            struct nodo *recoAux = sintaxisLeer(reco, 1);
            sintaxisIf(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "Mostrar") == 0) {
            edo = 6;
            struct nodo *recoAux = sintaxisEscribir(reco, 1);
            sintaxisIf(recoAux, edo);
        } else if (reco->info.tipo == TK_IDENTIFICADOR) {
            edo = 6;
            struct nodo *recoAux = sintaxisAsignar(reco, 1);
            sintaxisIf(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "mientras") == 0) {
            edo = 6;
            struct nodo *recoAux = sintaxisMientras(reco, 1);
            sintaxisIf(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "para") == 0) {
            edo = 6;
            struct nodo *recoAux = sintaxisPara(reco, 1);
            sintaxisIf(recoAux, edo);
        } else {
            edo = 7;
            sintaxisIf(reco, edo);
        }
    } else if (edo == 7) {
        if ((strcasecmp(reco->info.lexema, "}") == 0) && (strcasecmp(reco->der->info.lexema, "sino") == 0)) {
            edo = 8;
            struct nodo *recoAux = sintaxisSino(reco->der, 1);
            sintaxisIf(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "}") == 0) {
            edo = 8;
            sintaxisIf(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo LLAVE DE CIERRE ( } ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 8) {
        if (strcasecmp(reco->info.lexema, ";") != 0) {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo PUNTO Y COMA ( ; ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

struct nodo *sintaxisMientras(struct nodo *reco, int edo) {
    if (edo == 1) {
        if (strcasecmp(reco->info.lexema, "mientras") == 0) {
            edo = 2;
            sintaxisMientras(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba la palabra ( mientras ) en la línea %d columna %d.",
                   reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 2) {
        if (strcasecmp(reco->info.lexema, "(") == 0) {
            edo = 3;
            sintaxisMientras(reco->der, edo);
        } else {
            printf(
                "ERROR DE SINTAXIS :: Se esperaba el simbolo PARENTESIS DE APERTURA ( ( ) en la línea %d columna %d.",
                reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 3) {
        if ((strcasecmp(reco->info.lexema, "verdadero") == 0) || (strcasecmp(reco->info.lexema, "falso") == 0)) {
            edo = 4;
            sintaxisIf(reco->der, edo);
        } else {
            printf("Se esperaba un valor VERDADERO o FALSO en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 4) {
        if (strcasecmp(reco->info.lexema, ")") == 0) {
            edo = 5;
            sintaxisMientras(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo PARENTESIS DE CIERRE ( ) ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 5) {
        if (strcasecmp(reco->info.lexema, "{") == 0) {
            edo = 6;
            sintaxisMientras(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo LLAVE DE APERTURA ( { ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 6) {
        if (strcasecmp(reco->info.lexema, "mientras") == 0) {
            edo = 6;
            struct nodo *recoAux = sintaxisMientras(reco, 1);
            sintaxisMientras(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "si") == 0) {
            edo = 6;
            struct nodo *recoAux = sintaxisIf(reco, 1);
            sintaxisMientras(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "Leer") == 0) {
            edo = 6;
            struct nodo *recoAux = sintaxisLeer(reco, 1);
            sintaxisMientras(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "Mostrar") == 0) {
            edo = 6;
            struct nodo *recoAux = sintaxisEscribir(reco, 1);
            sintaxisMientras(recoAux, edo);
        } else if (reco->info.tipo == TK_IDENTIFICADOR) {
            edo = 6;
            struct nodo *recoAux = sintaxisAsignar(reco, 1);
            sintaxisMientras(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "para") == 0) {
            edo = 6;
            struct nodo *recoAux = sintaxisPara(reco, 1);
            sintaxisMientras(recoAux, edo);
        } else {
            edo = 7;
            sintaxisMientras(reco, edo);
        }
    } else if (edo == 7) {
        if (strcasecmp(reco->info.lexema, "}") == 0) {
            edo = 8;
            sintaxisMientras(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo LLAVE DE CIERRE ( } ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 8) {
        if (strcasecmp(reco->info.lexema, ";") != 0) {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo PUNTO Y COMA ( ; ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

struct nodo *sintaxisPara(struct nodo *reco, int edo) {
    if (edo == 1) {
        if (strcasecmp(reco->info.lexema, "para") == 0) {
            edo = 2;
            sintaxisPara(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba la palabra ( para ) en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 2) {
        if (strcasecmp(reco->info.lexema, "(") == 0) {
            edo = 3;
            sintaxisPara(reco->der, edo);
        } else {
            printf(
                "ERROR DE SINTAXIS :: Se esperaba el simbolo PARENTESIS DE APERTURA ( ( ) en la línea %d columna %d.",
                reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 3) {
        if (reco->info.tipo == TK_IDENTIFICADOR) {
            edo = 4;
            sintaxisPara(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba un IDENTIFICADOR en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 4) {
        if (strcasecmp(reco->info.lexema, ";") == 0) {
            edo = 5;
            sintaxisPara(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo PUNTO Y COMA ( ; ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 5) {
        if ((strcasecmp(reco->info.lexema, "verdadero") == 0) || (strcasecmp(reco->info.lexema, "falso") == 0)) {
            edo = 6;
            sintaxisPara(reco->der, edo);
        } else {
            printf("Se esperaba un valor VERDADERO o FALSO en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 6) {
        if (strcasecmp(reco->info.lexema, ";") == 0) {
            edo = 7;
            sintaxisPara(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo PUNTO Y COMA ( ; ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 7) {
        if (reco->info.tipo == TK_NUMERO) {
            edo = 8;
            sintaxisPara(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba un NUMERO en la línea %d columna %d.", reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 8) {
        if (strcasecmp(reco->info.lexema, ")") == 0) {
            edo = 9;
            sintaxisPara(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo PARENTESIS DE CIERRE ( ) ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 9) {
        if (strcasecmp(reco->info.lexema, "{") == 0) {
            edo = 10;
            sintaxisPara(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo LLAVE DE APERTURA ( { ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 10) {
        if (strcasecmp(reco->info.lexema, "para") == 0) {
            edo = 10;
            struct nodo *recoAux = sintaxisPara(reco, 1);
            sintaxisPara(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "mientras") == 0) {
            edo = 10;
            struct nodo *recoAux = sintaxisMientras(reco, 1);
            sintaxisPara(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "si") == 0) {
            edo = 10;
            struct nodo *recoAux = sintaxisIf(reco, 1);
            sintaxisPara(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "Leer") == 0) {
            edo = 10;
            struct nodo *recoAux = sintaxisLeer(reco, 1);
            sintaxisPara(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "Mostrar") == 0) {
            edo = 10;
            struct nodo *recoAux = sintaxisEscribir(reco, 1);
            sintaxisPara(recoAux, edo);
        } else if (reco->info.tipo == TK_IDENTIFICADOR) {
            edo = 10;
            struct nodo *recoAux = sintaxisAsignar(reco, 1);
            sintaxisPara(recoAux, edo);
        } else {
            edo = 11;
            sintaxisPara(reco, edo);
        }
    } else if (edo == 11) {
        if (strcasecmp(reco->info.lexema, "}") == 0) {
            edo = 12;
            sintaxisPara(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo LLAVE DE CIERRE ( } ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 12) {
        if (strcasecmp(reco->info.lexema, ";") != 0) {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo PUNTO Y COMA ( ; ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

void sintaxisPrograma(struct nodo *reco, int edo) {
    if (edo == 1) {
        if (strcasecmp(reco->info.lexema, "declaracion") == 0) {
            edo = 2;
            sintaxisPrograma(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba la palabra (declaracion) en la línea %d columna %d.",
                   reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 2) {
        if (strcasecmp(reco->info.nombre, "CORCHETE IZQ") == 0) {
            edo = 3;
            sintaxisPrograma(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo CORCHETE DE CIERRE ( [ ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 3) {
        if (reco->info.tipo == TK_TIPODATO) {
            edo = 3;
            struct nodo *recoAux = sintaxisVariable(reco, 1);
            sintaxisPrograma(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "CONSTANTE") == 0) {
            edo = 3;
            struct nodo *recoAux = sintaxisConstante(reco, 1);
            sintaxisPrograma(recoAux, edo);
        } else if (strcasecmp(reco->info.nombre, "CORCHETE DER") == 0) {
            edo = 4;
            sintaxisPrograma(reco, edo);
        } else if ((reco->info.tipo != 4) || (strcasecmp(reco->info.lexema, "CONSTANTE") != 0)) {
            printf(
                "ERROR DE SINTAXIS :: Se esperaba un TIPO DE DATO o la palabra Cconstante) en la línea %d columna %d.",
                reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 4) {
        if (strcasecmp(reco->info.nombre, "CORCHETE DER") == 0) {
            edo = 5;
            sintaxisPrograma(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo CORCHETE DE CIERRE ( ] ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 5) {
        if (strcasecmp(reco->info.lexema, "principal") == 0) {
            edo = 6;
            sintaxisPrograma(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba la palabra (principal) en la línea %d columna %d.",
                   reco->info.numLin,
                   reco->info.numCol);
        }
    } else if (edo == 6) {
        if (reco->der == NULL) {
            edo = 8;
            sintaxisPrograma(NULL, edo);
        } else if (strcasecmp(reco->info.nombre, "CORCHETE IZQ") == 0) {
            edo = 7;
            sintaxisPrograma(reco->der, edo);
        } else {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo CORCHETE DE APERTURA ( [ ) en la línea %d columna %d.",
                   reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 7) {
        if (strcasecmp(reco->info.lexema, "Leer") == 0) {
            edo = 7;
            struct nodo *recoAux = sintaxisLeer(reco, 1);
            if (recoAux == NULL) {
                edo = 8;
                sintaxisPrograma(NULL, edo);
            } else
                sintaxisPrograma(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "Mostrar") == 0) {
            edo = 7;
            struct nodo *recoAux = sintaxisEscribir(reco, 1);
            if (recoAux == NULL) {
                edo = 8;
                sintaxisPrograma(NULL, edo);
            } else
                sintaxisPrograma(recoAux, edo);
        } else if (reco->info.tipo == TK_IDENTIFICADOR) {
            edo = 7;
            struct nodo *recoAux = sintaxisAsignar(reco, 1);
            if (recoAux == NULL) {
                edo = 8;
                sintaxisPrograma(NULL, edo);
            } else
                sintaxisPrograma(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "si") == 0) {
            edo = 7;
            struct nodo *recoAux = sintaxisIf(reco, 1);
            if (recoAux == NULL) {
                edo = 8;
                sintaxisPrograma(NULL, edo);
            } else
                sintaxisPrograma(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "mientras") == 0) {
            edo = 7;
            struct nodo *recoAux = sintaxisMientras(reco, 1);
            if (recoAux == NULL) {
                edo = 8;
                sintaxisPrograma(NULL, edo);
            } else
                sintaxisPrograma(recoAux, edo);
        } else if (strcasecmp(reco->info.lexema, "para") == 0) {
            edo = 7;
            struct nodo *recoAux = sintaxisPara(reco, 1);
            if (recoAux == NULL) {
                edo = 8;
                sintaxisPrograma(NULL, edo);
            } else
                sintaxisPrograma(recoAux, edo);
        } else if (reco != NULL) {
            edo = 8;
            sintaxisPrograma(reco, edo);
        }
    } else {
        if (reco == NULL) {
            printf("ERROR DE SINTAXIS :: Se esperaba el simbolo CORCHETE DE CIERRE ( ] ) en la ultima linea");
        } else {
            printf("\n Fin del analisis sintactico. Todo correcto.");
        }
    }
}

#endif //DEFINICIONSINTACTICO_H
