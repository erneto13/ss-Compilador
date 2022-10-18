#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
//#include <locale.h>

typedef enum tipoToken{
    PalRes, //0
    ID, //1
    Num, //2
    Sim, //3
    TipoDato, //4
    Cadena, //5
    ERROR, //6
    Booleano //7
} tt;

struct Token{
    char nombre[255];
    enum tipoToken tipo;
    char lexema[255];
    float valor;
    int numLin;
    int numCol;
};

struct nodo{
    struct Token info;
    struct nodo *izq;
    struct nodo *der;
};

struct nodo *raiz = NULL;
struct nodo *act = NULL;

void limpiarToken(struct Token token){
    token.valor = 0;
    memset(token.lexema, 0, sizeof(token.lexema));
    memset(token.nombre, 0, sizeof(token.nombre));
    token.tipo = 0;
    token.numCol = 0;
    token.numLin = 0;
}

void insertar(struct Token token)
{
    struct nodo *nuevo;
    nuevo = malloc(sizeof(struct nodo));
    nuevo->info = token;
    nuevo->izq = NULL;
    nuevo->der = NULL;
    if (raiz == NULL)
    {
        raiz = nuevo;
        act = nuevo;
    }
    else
    {
        nuevo->izq = act;
        act->der = nuevo;
        act = nuevo;
    }
}

void imprimirLista(struct nodo *reco)
{
    if (reco != NULL)
    {
        printf("Nombre: %s\t\t", reco->info.nombre);
        printf("Tipo: %d\t", reco->info.tipo);
        switch (reco->info.tipo) {
            case 0:{
                printf("Tipo: PalRes\t");
                break;
            }
            case 1:{
                printf("Tipo: ID\t");
                break;
            }
            case 2:{
                printf("Tipo: Num\t");
                break;
            }
            case 3:{
                printf("Tipo: Sim\t");
                break;
            }
            case 4:{
                printf("Tipo: TipoDato\t");
                break;
            }
            case 5:{
                printf("Tipo: Cadena\t");
                break;
            }
            case 6:{
                printf("Tipo: ERROR\t");
                break;
            }
        }
        printf("Lexema: %s\t\t", reco->info.lexema);
        printf("Valor: %.2f ", reco->info.valor);
        printf("Linea: %d Columna: %d\n", reco->info.numLin, reco->info.numCol);
        imprimirLista(reco->der);
    }

}

int tamArchivo(FILE *f){        // Encuentra el total de caracteres en el archivo moviendo el puntero que lo lee, al final reinicia el puntero a su posición original.
    size_t pos = ftell(f);
    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    fseek(f, pos, SEEK_SET);
    return length;
}

bool esLimitador(char c){
    if(c == ' ' || c == ';' || c == '[' || c == ']' || c == '(' || c == ')' || c == '\n' || c == EOF || c == '\t' ||
       c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '=' || c == '|' || c == '&' ||
       c == '{' || c == '}'){
        return true;
    }
    return false;
}

bool esTerminador(char c, struct Token *token){
    //if(c == ';' || c == '[' || c == ']' || c == '(' || c == ')' || c == '"' || c == '\n' || c == EOF){
    if(c == ';' || c == '[' || c == ']' || c == '(' || c == ')' || c == '"'|| c == '|'|| c == '&' || c == '{' || c == '}'){
        //strcpy(token->lexema, NULL);
        memset(token->lexema, 0, sizeof(token->lexema));
        token->lexema[0] = c;
        token->tipo = Sim;
        token->valor = 0;
        switch (c) {
            case ';':{
                strcpy(token->nombre, "PuntoYComa");
                break;
            }
            case '[':{
                strcpy(token->nombre, "CorcheteIzq");
                break;
            }
            case ']':{
                strcpy(token->nombre, "CorcheteDer");
                break;
            }
            case '(':{
                strcpy(token->nombre, "ParentesisIzq");
                break;
            }
            case ')':{
                strcpy(token->nombre, "ParentesisDer");
                break;
            }
            case '"':{
                strcpy(token->nombre, "ComillaDoble");
                break;
            }
            case '|':{
                strcpy(token->nombre, "OR");
                break;
            }case '&':{
                strcpy(token->nombre, "AND");
                break;
            }case '{':{
                strcpy(token->nombre, "LlaveIzq");
                break;
            }case '}':{
                strcpy(token->nombre, "LlaveDer");
                break;
            }
            default:
                break;
        }
        return true;
    }
    return false;
}

bool tipoDato(char* str, struct Token *token){
    if(strcmp(str,"entero") == 0 || strcmp(str,"real") == 0 || strcmp(str,"cadena") == 0 || strcmp(str,"caracter") == 0 || strcmp(str,"booleano") == 0){
        strcpy(token->lexema, str);
        token->tipo = TipoDato;
        token->valor = 0;
        strcpy(token->nombre, "Tipo de Dato");
        return true;
    }
    return false;
}

bool operador(char c, struct Token *token){
    if(c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '>' || c == '<'){
        //strcpy(token->lexema, NULL);
        memset(token->lexema, 0, sizeof(token->lexema));
        token->lexema[0] = c;
        token->valor = 0;
        token->tipo = Sim;
        switch (c) {
            case '+':{
                strcpy(token->nombre, "Mas");
                break;
            }
            case '-':{
                strcpy(token->nombre, "Menos");
                break;
            }
            case '*':{
                strcpy(token->nombre, "Por");
                break;
            }
            case '/':{
                strcpy(token->nombre, "Entre");
                break;
            }
            case '=':{
                strcpy(token->nombre, "Igual");
                break;
            }
            case '>':{
                strcpy(token->nombre, "Mayor");
                break;
            }
            case '<':{
                strcpy(token->nombre, "Menor");
                break;
            }
            default:
                break;
        }
        return true;
    }
    return false;
}

bool esNumero(char* str, struct Token *token){
    int z = 1;
    int punto = 0;
    //int as = sizeof(str);
    if(str[0] >= '0' && str[0]<= '9'){
        while(z <= strlen(str)-1){
            if(str[z] == '.'){
                punto++;
            }
            if(!(str[z] == '.' || (str[z] >= '0' && str[z] <= '9')) && punto == 0){
                return false;
            }
            z++;
        }
        strcpy(token->lexema, str);
        token->tipo = Num;
        strcpy(token->nombre, "Constante");
        token->valor = atof(str); // NOLINT(cert-err34-c,cppcoreguidelines-narrowing-conversions)
        return true;
    }
    return false;
}

bool palReservada(char* str, struct Token *token){
    if(strcmp(str, "declaracion") == 0 || strcmp(str, "principal") == 0 || strcmp(str, "regresa") == 0 || strcmp(str, "Mostrar") == 0 ||
       strcmp(str, "Leer") == 0  || strcmp(str, "si") == 0  || strcmp(str, "entonces") == 0  || strcmp(str, "para") == 0  || strcmp(str, "mientras") == 0 || strcmp(str, "sino") == 0){
        //if(str == "declaracion" || str == "principal" || str == "regresa" || str == "Mostrar" || str == "Leer"){
        strcpy(token->lexema, str);
        token->tipo = PalRes;
        strcpy(token->nombre, "Instruccion");
        token->valor = 0;
        return true;
    }
    return false;
}

bool esBool(char* str, struct Token *token){
    if(strcmp(str, "verdadero") == 0 || strcmp(str, "falso") == 0){
        strcpy(token->lexema, str);
        token->tipo = Booleano;
        strcpy(token->nombre, str);
        token->valor = 0;
        return true;
    }
    return false;
}

bool esIdentificador(char* str, struct Token *token){
    if(str[0] >= 'a' && str[0] <= 'z'){
        int z = 1;
        bool aux;
        while(z <= strlen(str)-1){
            char c = str[z];
            if(!((c >= 'A' && c<= 'Z') || (c >= 'a' && c<= 'z') || (c >= '0' && c<= '9') || c == '_')){
                return false;
            }
            z++;
        }
        strcpy(token->lexema, str);
        token->tipo = ID;
        strcpy(token->nombre, "Identificador");
        token->valor = 0;
        return true;
    }
    return false;
}

bool esCadena(char* str, struct Token *token){
    char c = str[0];
    char z = str[strlen(str)-1];
    if(str[0] == '"' && (str[strlen(str)-1] == '"' || str[strlen(str)-1 == ' '])){
        strcpy(token->lexema, str);
        token->tipo = Cadena;
        strcpy(token->nombre, "Cadena");
        token->valor = 0;
        return true;
    }
    else{
        return false;
    }
    /*int z = 1;
    bool aux;
    while(z <= strlen(str)-1){
        char c = str[z];
        if(isascii(c) == 0){
            return false;
        }
        z++;
    }

    return true;
return false;*/
}

void leerTokens(char* prog){
    //int der = 0;
    //int izq = 0;
    //int right[sizeof(prog)], left[sizeof(prog)];
    //char lista[sizeof(prog)][4];        // 1: token, 2: izq, 3: der, 4: tipo token
    int lines = 1;
    int columna = 0;
    char aux[255];
    memset(aux, 0, sizeof(aux));
    int x = 0;
    int y = 0;
    struct Token token;
    bool cadena = false;
    int cadenaAux = 0;
    //token.valor = NULL;
    while(prog[x] != EOF){
        if(prog[x] == '\n'){
            lines++;
            columna = 0;
        }
        char c = prog[x];
        if(cadena == true && cadenaAux != 2){
            if(prog[x] == '"'){
                cadena = true;
                cadenaAux++;
            }
            aux[y] = prog[x];
            y++;
        }
        else if(cadenaAux == 2){
            if(esCadena(aux, &token)){
                cadena = false;
                cadenaAux = 0;
                token.numCol = columna;
                token.numLin = lines;
                insertar(token);
                limpiarToken(token);
                //printf("Tipo: Error (cadena)\n");
            }
            else if(aux[0] == '"'){
                token.tipo = ERROR;
                strcpy(token.nombre, "Error Comilla");
                token.valor = 0;
                strcpy(token.lexema, "Error Comilla");
                token.numCol = columna;
                token.numLin = lines;
                insertar(token);
                limpiarToken(token);
            }
            memset(aux, 0, sizeof(aux));
            y = 0;
            continue;
        }
        else{
            if(esLimitador(prog[x])) {
                //if(esLimitador(prog[x]) && !(cadena == true && cadenaAux == 2)) {
                if(y!=0){
                    //printf("Pos: %d  Token: %s\n", x, aux);
                    /*if(aux[y] == '\n'){
                        printf("salto de línea");
                    }*/
                    //printf("\naux: %c", aux[y]);
                    if(tipoDato(aux, &token)){
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                        //printf("Tipo: Tipo de Dato\n");
                        //token.nombre = "";
                        //token.tipo = TipoDato;
                    }
                    else if(palReservada(aux, &token)){
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                        //printf("Tipo: Palabra Reservada\n");
                    }
                    else if(esBool(aux, &token)){
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                        //printf("Tipo: Boolean\n");
                    }
                    else if (esIdentificador(aux, &token)){
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                        //printf("Tipo: Identificador\n");
                    }
                    else if (esNumero(aux, &token)){
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                        //printf("Tipo: Numero\n");
                    }
                        /*else if(esCadena(aux, &token)){
                            cadena = false;
                            cadenaAux = 0;
                            insertar(token);
                            limpiarToken(token);
                            //printf("Tipo: Error (cadena)\n");
                        }*/
                    else if(aux[0] == '"'){
                        token.tipo = ERROR;
                        strcpy(token.nombre, "Error Comilla");
                        token.valor = 0;
                        strcpy(token.lexema, "Error Comilla");
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                    }
                    else{
                        token.tipo = ERROR;
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
                }
                else{
                    if(operador(prog[x], &token)){
                        //printf("Pos: %d  Token: %c\t", x, prog[x]);
                        //printf("tipo: Operador.\n");
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                        //x++;
                        //continue;
                    }
                    else if(esTerminador(prog[x], &token)){
                        if(prog[x] == '\n'){
                            //printf("Pos: %d  Token: \\n\t", x);
                            lines++;
                            columna = 0;
                        }
                        else if(prog[x] == EOF){
                            //printf("Pos: %d  Token: EOF\t", x);
                        }
                        else{
                            //printf("Pos: %d  Token: %c\t", x, prog[x]);
                        }
                        //printf("tipo: Limitador.\n");
                        token.numCol = columna;
                        token.numLin = lines;
                        insertar(token);
                        limpiarToken(token);
                        //x++;
                        //continue;
                    }

                }
            }
            else{
                if(prog[x] == '"'){
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
    if(cadena == true){
        token.tipo = ERROR;
        strcpy(token.nombre, "ERROR Comilla");
        token.valor = 0;
        strcpy(token.lexema, "ERROR Comilla");
        token.numCol = columna;
        token.numLin = lines;
        insertar(token);
        limpiarToken(token);
    }
}

struct nodo *sintaxisExp(struct nodo *reco, int edo){
    if ( edo == 1 ){
        if ( reco->info.tipo == ID || reco->info.tipo == Num ){
            /*if(strcmp(reco->info.lexema, ";")){
                edo = 5;
                sintaxisExp(reco, edo);
            }
            else{
                edo = 2;
                sintaxisExp(reco->der, edo);
            }*/
            edo = 2;
            sintaxisExp(reco->der, edo);
        }
        else if(strcmp(reco->info.lexema, "(") == 0){
            edo = 3;
            sintaxisExp(reco->der, edo);
        }
        /*else if(strcmp(reco->info.lexema, ";")){
            edo = 5;
            sintaxisExp(reco, edo);
        }*/
        else {
            printf("Error de sintaxis: Se esperaba un Identificador, un Número o un paréntesis en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    }
    else if ( edo == 2 ) {
        if ( strcmp(reco->info.lexema, "+") == 0 || strcmp(reco->info.lexema, "-") == 0 || strcmp(reco->info.lexema, "*") == 0 || strcmp(reco->info.lexema, "/") == 0 ) {
            edo = 1;
            sintaxisExp(reco->der, edo);
        }
        else if(strcmp(reco->info.lexema, ")") == 0){
            return reco;
        }
        else if(strcmp(reco->info.lexema, ";") == 0){
            edo = 5;
            sintaxisExp(reco, edo);
        }
        else {
            printf("Error de sintaxis: Se esperaba un símbolo operador en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    }
    else if ( edo == 3 ) {
        edo = 1;
        //struct nodo *recoAux = sintaxisExp(reco, edo);
        reco = sintaxisExp(reco, edo);
        if(strcmp(reco->info.lexema, ")") == 0){
            edo = 2;
            sintaxisExp(reco->der, edo);
        }
        else{
            printf("Error de sintaxis: Se esperaba un parentesis \")\" derecho en la linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    }
    else if ( edo == 5 ) {
        if ( strcasecmp(reco->info.lexema, ";") != 0 ) {
            printf("321Error de sintaxis: Se esperaba el símbolo Punto y coma ( ; ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
        else {
            return (reco);
        }
    }
}

struct nodo * sintaxisVariable(struct nodo *reco, int edo) {
    if ( edo == 1 ){
        if( reco->info.tipo == TipoDato) {
            edo = 2;
            sintaxisVariable(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba un Tipo de Dato en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 2 ) {
        if ( reco->info.tipo == ID ){
            edo = 3;
            sintaxisVariable(reco->der, edo);
        } else{
            printf("Error de sintaxis: Se esperaba un Identificador en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 3 ){
        if ( strcasecmp(reco->info.lexema, "=") == 0){
            edo = 4;
            sintaxisVariable(reco->der, edo);
        } else{
            printf("Error de sintaxis: Se esperaba el simbolo de igualdad ( = ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 4 ){
        if((reco->info.tipo == Cadena) || (reco->info.tipo == Num) || (reco->info.tipo == Booleano)){
            edo = 5;
            sintaxisVariable(reco->der, edo);
        }
        else if(reco->info.tipo == ID) {
            printf("Error de sintaxis: Una variable sólo se puede inicializar con un valor constante, en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
        else {
            printf("Error de sintaxis: Debe asignarse un valor a la variable en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 5){
        if ( strcasecmp(reco->info.lexema, ";") != 0 ) {
            printf("Error de sintaxis: Se esperaba el simbolo Punto y coma ( ; ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

struct nodo * sintaxisConstante(struct nodo *reco, int edo) {

    if( edo == 1 ) {
        if ( strcasecmp(reco->info.lexema, "constante") == 0) {
            edo = 2;
            sintaxisConstante(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba la palabra (constante) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    }else if ( edo == 2 ){
        if( reco->info.tipo == TipoDato) {
            edo = 3;
            sintaxisVariable(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba un Tipo de Dato en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 3 ) {
        if ( reco->info.tipo == ID ){
            edo = 4;
            sintaxisVariable(reco->der, edo);
        } else{
            printf("Error de sintaxis: Se esperaba un Identificador en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 4 ){
        if ( strcasecmp(reco->info.lexema, "=") == 0){
            edo = 5;
            sintaxisVariable(reco->der, edo);
        } else{
            printf("Error de sintaxis: Se esperaba el simbolo de igualdad ( = ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 5 ){
        if (reco->info.tipo == Num) {
            edo = 6;
            sintaxisVariable(reco->der, edo);
        } else if ( reco->info.tipo == Cadena ) {
            edo = 6;
            sintaxisVariable(reco->der, edo);
        } else if ( reco->info.tipo == Booleano ) {
            edo = 6;
            sintaxisVariable(reco->der, edo);
        } else {
            printf("Error de sintaxis: Debe asignarse un valor a la variable en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if (edo == 6){
        if ( strcasecmp(reco->info.lexema, ";") != 0 ) {
            printf("Error de sintaxis: Se esperaba el simbolo Punto y coma ( ; ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

struct nodo *sintaxisLeer(struct nodo *reco, int edo){
    if ( edo == 1 ){
        if ( strcasecmp(reco->info.lexema, "Leer") == 0 ){
            edo = 2;
            sintaxisLeer(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba la palabra (Leer) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 2 ) {
        if ( strcasecmp(reco->info.lexema, "(") == 0 ) {
            edo = 3;
            sintaxisLeer(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Parentesis de apertura ( ( )  en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 3 ) {
        //printf(reco->info.lexema);
        if ( reco->info.tipo == 1 ){ //Espera recibir un identificador
            edo = 4;
            sintaxisLeer(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba un Identificador Leer en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 4 ) {
        if ( strcasecmp(reco->info.lexema, ")") == 0 ) {
            edo = 5;
            sintaxisLeer(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Parentesis de cierre ( ) ) Leer  en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 5 ) {
        if ( strcasecmp(reco->info.lexema, ";") != 0 ) {
            printf("Error de sintaxis: Se esperaba el simbolo Punto y coma ( ; ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

struct nodo *sintaxisEscribir(struct nodo *reco, int edo){
    if ( edo == 1 ) {
        if ( strcasecmp(reco->info.lexema, "Mostrar") == 0 ){
            edo = 2;
            sintaxisEscribir(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba la palabra (Mostrar) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 2) {
        if ( strcasecmp(reco->info.lexema, "(") == 0 ) {
            edo = 3;
            sintaxisEscribir(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Parentesis de apertura ( ( )  en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 3 ) {
        if ( (reco->info.tipo == 1) || (reco->info.tipo == 5 ) || ( strcasecmp(reco->info.lexema, "+") == 0 ) ){
            //Espera recibir una cadena en tipo 5 o un ID en tipo 1 o el simbolo "+" para concatenaciones
            edo = 3;
            sintaxisEscribir(reco->der, edo);
        } else if ( !( (reco->info.tipo == 1) || (reco->info.tipo == 5 ) || ( strcasecmp(reco->info.lexema, "+") == 0 ) ) ) {
            edo = 4;
            sintaxisEscribir(reco, edo);
        }else {
            printf("Error de sintaxis: Se esperaba una Cadena o un Identificador en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 4 ) {
        if ( strcasecmp(reco->info.lexema, ")") == 0 ) {
            edo = 5;
            sintaxisEscribir(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Parentesis de cierre ( ) ) Mostrar en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 5 ) {
        if ( strcasecmp(reco->info.lexema, ";") != 0 ) {
            printf("Error de sintaxis: Se esperaba el simbolo Punto y coma ( ; ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

// TODO: Add Expresión.
struct nodo *sintaxisAsignar(struct nodo *reco, int edo) {
    if ( edo == 1 ) {
        if ( reco->info.tipo == ID ) { //Espera un ID
            edo = 2;
            sintaxisAsignar(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba un Identificador en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 2 ) {
        if ( strcasecmp(reco->info.lexema, "=") == 0 ) {
            edo = 3;
            sintaxisAsignar(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo de Asignacion ( = ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 3 ) {
        //printf("\n %d %d\n", reco->info.tipo,  strcmp(reco->der->info.lexema, "-"));
        if(reco->info.tipo == ID || strcasecmp(reco->info.lexema, "(") == 0 || ((reco->info.tipo == Num || reco->info.tipo == Cadena) && (strcmp(reco->der->info.lexema, "+") == 0 || strcmp(reco->der->info.lexema, "-") == 0 || strcmp(reco->der->info.lexema, "*") == 0 || strcmp(reco->der->info.lexema, "/") == 0 || strcmp(reco->der->info.lexema, "(") == 0))) {
            //struct nodo *recoAux = sintaxisExp(reco, 1);
            edo = 4;
            struct nodo *recoAux = sintaxisExp(reco, 1);
            sintaxisAsignar(recoAux, edo);
            //*reco = *sintaxisExp(reco, 1);
            /*if(strcmp(recoAux->info.lexema, ";") == 0){
                edo = 4;
                sintaxisAsignar(reco, edo);
            }
            else{
                printf("\n %d %s\n", reco->info.tipo, reco->der->info.lexema);
                printf("123 Error de sintaxis: Se esperaba el simbolo Punto y coma ( ; ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
            }*/
        }
        else if((reco->info.tipo == Cadena) || (reco->info.tipo == Num) || (reco->info.tipo == Booleano)){
            //Espera recibir una Cadena en tipo 5 o un numero en tipo 2
            edo = 4;
            sintaxisAsignar(reco->der, edo);
        }
        else {
                printf("Error de sintaxis: Se esperaba una asignación en la linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 4 ) {
        if ( strcasecmp(reco->info.lexema, ";") != 0 ) {
            printf("Error de sintaxis: Se esperaba el simbolo Punto y coma ( ; ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

struct nodo *sintaxisIf(struct nodo *reco, int edo);
struct nodo *sintaxisMientras(struct nodo *reco, int edo);
struct nodo *sintaxisPara(struct nodo *reco, int edo);

struct nodo *sintaxisSino(struct nodo *reco, int edo) {
    if ( edo == 1) {
        if ( strcasecmp(reco->info.lexema, "sino" ) == 0 ){
            edo = 2;
            sintaxisSino(reco->der, edo);
        }else {
            printf("Error de sintaxis: Se esperaba la palabra ( sino ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 2 ) {
        if ( strcasecmp(reco->info.lexema, "{" ) == 0 ){
            edo = 3;
            sintaxisSino(reco->der, edo);
        }else {
            printf("Error de sintaxis: Se esperaba el simbolo Llave de apertura ( { ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 3 ) {
        //Espacio para cualquier tipo de acción
        if ( strcasecmp(reco->info.lexema, "si" ) == 0 ) {
            edo = 3;
            struct nodo *recoAux = sintaxisIf(reco, 1);
            sintaxisSino(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "Leer" ) == 0 ) {
            edo = 3;
            struct nodo *recoAux = sintaxisLeer(reco, 1);
            sintaxisSino(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "Mostrar" ) == 0 ) {
            edo = 3;
            struct nodo *recoAux = sintaxisEscribir(reco, 1);
            sintaxisSino(recoAux, edo);
        }else if ( reco->info.tipo == ID ) { //Recibe un ID
            edo = 3;
            struct nodo *recoAux = sintaxisAsignar(reco, 1);
            sintaxisSino(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "mientras" ) == 0 ) {
            edo = 3;
            struct nodo *recoAux = sintaxisMientras(reco, 1);
            sintaxisSino(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "para" ) == 0 ) {
            edo = 3;
            struct nodo *recoAux = sintaxisPara(reco, 1);
            sintaxisSino(recoAux, edo);
        }else {  
            edo = 4;
            sintaxisSino(reco, edo);
        }

    } else if ( edo == 4) {
        if ( strcasecmp(reco->info.lexema, "}" ) == 0 ){
            return (reco->der);
        }else {
            printf("Error de sintaxis: Se esperaba el simbolo Llave de cierre ( } ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    }
}

struct nodo *sintaxisIf(struct nodo *reco, int edo){
    //printf(reco->info.lexema);
    if ( edo == 1 ) {
        if ( strcasecmp(reco->info.lexema, "si" ) == 0 ) {
            edo = 2;
            sintaxisIf(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba la palabra ( si ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 2 ) {
        if ( strcasecmp(reco->info.lexema, "(" ) == 0 ) {
            edo = 3;
            sintaxisIf(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Parentesis de apertura ( ( ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 3 ) {
        //Espacio para expresion verdadera o falsa
        if ( (strcasecmp(reco->info.lexema, "verdadero" ) == 0) ||  (strcasecmp(reco->info.lexema, "falso" ) == 0) ){
            edo = 4;
            sintaxisIf(reco->der, edo);
        } else {
            printf("Se esperaba un valor Verdadero o Falso en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }

    } else if ( edo == 4 ) {
        if ( strcasecmp(reco->info.lexema, ")" ) == 0 ) {
            edo = 5;
            sintaxisIf(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Parentesis de cierre ( ) ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 5 ) {
        if ( strcasecmp(reco->info.lexema, "{" ) == 0 ) {
            edo = 6;
            sintaxisIf(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Llave de apertura ( { ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 6 ) {
        //Espacio para cualquier tipo de accion

        if ( strcasecmp(reco->info.lexema, "si" ) == 0 ) {
            edo = 6;
            struct nodo *recoAux = sintaxisIf(reco, 1);
            sintaxisIf(recoAux, edo);						 
        } else if ( strcasecmp(reco->info.lexema, "Leer" ) == 0 ) {
            edo = 6;
            struct nodo *recoAux = sintaxisLeer(reco, 1);
            sintaxisIf(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "Mostrar" ) == 0 ) {
            edo = 6;
            struct nodo *recoAux = sintaxisEscribir(reco, 1);
            sintaxisIf(recoAux, edo);
        }else if ( reco->info.tipo == ID ) { //Recibe un ID
            edo = 6;
            struct nodo *recoAux = sintaxisAsignar(reco, 1);
            sintaxisIf(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "mientras" ) == 0 ){
            edo = 6;
            struct nodo *recoAux = sintaxisMientras(reco, 1);
            sintaxisIf(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "para" ) == 0 ) {
            edo = 6;
            struct nodo *recoAux = sintaxisPara(reco, 1);
            sintaxisIf(recoAux, edo);
        }else { 
            edo = 7;
            sintaxisIf(reco, edo);
        }
    } else if ( edo == 7 ) {
        //printf(reco->info.lexema);
        if ( (strcasecmp(reco->info.lexema, "}" ) == 0) && (strcasecmp(reco->der->info.lexema, "sino" ) == 0) ) {
            edo = 8;
            struct nodo *recoAux = sintaxisSino(reco->der, 1);
            sintaxisIf(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "}" ) == 0 ){
            edo = 8;
            sintaxisIf(reco->der, edo);
        }else {
            printf("Error de sintaxis: Se esperaba el simbolo Llave de cierre ( } ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);																																	   
        }		 
    } else if ( edo == 8 ) {							 
        if ( strcasecmp(reco->info.lexema, ";") != 0 ) {
            printf("Error de sintaxis: Se esperaba el simbolo Punto y coma ( ; ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

struct nodo *sintaxisMientras(struct nodo *reco, int edo) {
    if ( edo == 1 ) {
        if ( strcasecmp(reco->info.lexema, "mientras") == 0 ) {
            edo = 2;
            sintaxisMientras(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba la palabra ( mientras ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 2 ) {
        if ( strcasecmp(reco->info.lexema, "(") == 0 ) {
            edo = 3;
            sintaxisMientras(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Parentesis de apertura ( ( ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 3 ) {
        if ( (strcasecmp(reco->info.lexema, "verdadero" ) == 0) ||  (strcasecmp(reco->info.lexema, "falso" ) == 0) ){
            edo = 4;
            sintaxisIf(reco->der, edo);
        } else {
            printf("Se esperaba un valor Verdadero o Falso en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 4 ) {
        if ( strcasecmp(reco->info.lexema, ")") == 0 ) {
            edo = 5;
            sintaxisMientras(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Parentesis de cierre ( ) ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 5 ) {
        if ( strcasecmp(reco->info.lexema, "{") == 0 ) {
            edo = 6;
            sintaxisMientras(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Llave de apertura ( { ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 6 ) {
        //Espacio para cualquier tipo de acción
        if ( strcasecmp(reco->info.lexema, "mientras" ) == 0 ) {
            edo = 6;
            struct nodo *recoAux = sintaxisMientras(reco, 1);
            sintaxisMientras(recoAux, edo);
        }else if ( strcasecmp(reco->info.lexema, "si" ) == 0 ) {
            edo = 6;
            struct nodo *recoAux = sintaxisIf(reco, 1);
            sintaxisMientras(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "Leer" ) == 0 ) {
            edo = 6;
            struct nodo *recoAux = sintaxisLeer(reco, 1);
            sintaxisMientras(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "Mostrar" ) == 0 ) {
            edo = 6;
            struct nodo *recoAux = sintaxisEscribir(reco, 1);
            sintaxisMientras(recoAux, edo);
        } else if ( reco->info.tipo == ID ) { //Recibe un ID
            edo = 6;
            struct nodo *recoAux = sintaxisAsignar(reco, 1);
            sintaxisMientras(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "para" ) == 0 ){
            edo = 6;
            struct nodo *recoAux = sintaxisPara(reco, 1);
            sintaxisMientras(recoAux, edo);
        } else {
            edo = 7;
            sintaxisMientras(reco, edo);
        }

    } else if ( edo == 7 ) {
        if ( strcasecmp(reco->info.lexema, "}") == 0 ) {
            edo = 8;
            sintaxisMientras(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Llave de cierre ( } ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 8 ) {
        if ( strcasecmp(reco->info.lexema, ";") != 0 ) {
            printf("Error de sintaxis: Se esperaba el simbolo Punto y coma ( ; ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

struct nodo *sintaxisPara(struct nodo *reco, int edo){
    if ( edo == 1 ) {
        if ( strcasecmp(reco->info.lexema, "para" ) == 0 ) {
            edo = 2;
            sintaxisPara(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba la palabra ( para ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 2 ) {
        if (strcasecmp(reco->info.lexema, "(") == 0) {
            edo = 3;
            sintaxisPara(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Parentesis de apertura ( ( ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 3 ) {
        if ( reco->info.tipo == ID ) { //Detecta un Identificador
            edo = 4;
            sintaxisPara(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba un Identificador en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 4 ) {
        if ( strcasecmp(reco->info.lexema, ";") == 0 ) {
            edo = 5;
            sintaxisPara(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Punto y coma ( ; ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 5 ){
        if ( (strcasecmp(reco->info.lexema, "verdadero" ) == 0) ||  (strcasecmp(reco->info.lexema, "falso" ) == 0) ) {
            edo = 6;
            sintaxisPara(reco->der, edo);
        } else {
            printf("Se esperaba un valor Verdadero o Falso en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 6 ) {
        if ( strcasecmp(reco->info.lexema, ";") == 0 ) {
            edo = 7;
            sintaxisPara(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Punto y coma ( ; ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 7 ) {
        if ( reco->info.tipo == Num ) {
            edo = 8;
            sintaxisPara(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba un Numero en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 8 ) {
        if ( strcasecmp(reco->info.lexema, ")") == 0 ) {
            edo = 9;
            sintaxisPara(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Parentesis de cierre ( ) ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 9 ) {
        if ( strcasecmp(reco->info.lexema, "{") == 0 ) {
            edo = 10;
            sintaxisPara(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Llave de apertura ( { ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 10 ) {
        //Espacio para cualquier tipo de accion

        if ( strcasecmp(reco->info.lexema, "para" ) == 0 ) {
            edo = 10;
            struct nodo *recoAux = sintaxisPara(reco, 1);
            sintaxisPara(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "mientras" ) == 0 ) {
            edo = 10;
            struct nodo *recoAux = sintaxisMientras(reco, 1);
            sintaxisPara(recoAux, edo);
        }else if ( strcasecmp(reco->info.lexema, "si" ) == 0 ) {
            edo = 10;
            struct nodo *recoAux = sintaxisIf(reco, 1);
            sintaxisPara(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "Leer" ) == 0 ) {
            edo = 10;
            struct nodo *recoAux = sintaxisLeer(reco, 1);
            sintaxisPara(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "Mostrar" ) == 0 ) {
            edo = 10;
            struct nodo *recoAux = sintaxisEscribir(reco, 1);
            sintaxisPara(recoAux, edo);
        }else if ( reco->info.tipo == ID ) { //Recibe un ID
            edo = 10;
            struct nodo *recoAux = sintaxisAsignar(reco, 1);
            sintaxisPara(recoAux, edo);
        } else {
            edo = 11;
            sintaxisPara(reco, edo);
        }

    } else if ( edo == 11 ) {
        if ( strcasecmp(reco->info.lexema, "}") == 0 ) {
            edo = 12;
            sintaxisPara(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Llave de cierre ( } ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 12 ) {
        if ( strcasecmp(reco->info.lexema, ";") != 0 ) {
            printf("Error de sintaxis: Se esperaba el simbolo Punto y coma ( ; ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        } else {
            return (reco->der);
        }
    }
}

void sintaxisPrograma(struct nodo *reco, int edo){
    if ( edo == 1 ) {
        if ( strcasecmp(reco->info.lexema, "declaracion") == 0 )  {
            edo = 2;
            sintaxisPrograma(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba la palabra (declaracion) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 2 ) {
        if ( strcasecmp(reco->info.nombre, "CorcheteIzq") == 0){
            edo = 3;
            sintaxisPrograma(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Corchete de apertura ( [ ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 3 ){
        if ( reco->info.tipo == TipoDato ){ //Detecta un tipo de dato
            edo = 3;
            //printf(recoAux->info.lexema);
            //printf("\n");
            struct nodo *recoAux = sintaxisVariable(reco, 1);
            sintaxisPrograma(recoAux, edo);

        } else if ( strcasecmp(reco->info.lexema, "constante" ) == 0) {
            edo = 3;
            struct nodo *recoAux = sintaxisConstante(reco, 1);
            sintaxisPrograma(recoAux, edo);
        } else if ( strcasecmp(reco->info.nombre, "CorcheteDer") == 0 ){
            edo = 4;
            sintaxisPrograma(reco, edo);
        } else if ( ( reco->info.tipo != 4 ) || ( strcasecmp(reco->info.lexema, "constante" ) != 0) ){
            printf("Error de sintaxis: Se esperaba un Tipo de Dato o la palabra (constante) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 4 ) {
        //printf(reco->info.lexema);
        if ( strcasecmp(reco->info.nombre, "CorcheteDer") == 0) {
            edo = 5;
            sintaxisPrograma(reco->der, edo);
        } else {
            //printf("Estado 4 \n");
            printf("Error de sintaxis: Se esperaba el simbolo Corchete de cierre ( ] ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 5 ){
        //printf(reco->info.lexema);
        if ( strcasecmp(reco->info.lexema, "principal") == 0){
            edo = 6;
            sintaxisPrograma(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba la palabra (principal) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 6) {
        if(reco->der == NULL){
            edo = 8;
            sintaxisPrograma(NULL, edo);
        }
        else if ( strcasecmp(reco->info.nombre, "CorcheteIzq") == 0){
            edo = 7;
            sintaxisPrograma(reco->der, edo);
        } else {
            printf("Error de sintaxis: Se esperaba el simbolo Corchete de apertura ( [ ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
        }
    } else if ( edo == 7) { //Cuerpo del programa
        if ( strcasecmp(reco->info.lexema, "Leer") == 0 ) {
            edo = 7;
            struct nodo *recoAux = sintaxisLeer(reco, 1);
            //printf(recoAux->info.lexema);
            if(recoAux == NULL){
                edo = 8;
                sintaxisPrograma(NULL, edo);
            }
            else
                sintaxisPrograma(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "Mostrar") == 0 ) {
            edo = 7;
            struct nodo *recoAux = sintaxisEscribir(reco, 1);
            if(recoAux == NULL){
                edo = 8;
                sintaxisPrograma(NULL, edo);
            }
            else
                sintaxisPrograma(recoAux, edo);
        } else if ( reco->info.tipo == ID){ //Espera recibir un identificador
            edo = 7;
            struct nodo *recoAux = sintaxisAsignar(reco, 1);
            if(recoAux == NULL){
                edo = 8;
                sintaxisPrograma(NULL, edo);
            }
            else
                sintaxisPrograma(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "si") == 0 ){
            edo = 7;
            struct nodo *recoAux = sintaxisIf(reco, 1);
            if(recoAux == NULL){
                edo = 8;
                sintaxisPrograma(NULL, edo);
            }
            else
                sintaxisPrograma(recoAux, edo);
        }else if ( strcasecmp(reco->info.lexema, "mientras") == 0 ){
            edo = 7;
            struct nodo *recoAux = sintaxisMientras(reco, 1);
            if(recoAux == NULL){
                edo = 8;
                sintaxisPrograma(NULL, edo);
            }
            else
                sintaxisPrograma(recoAux, edo);
        } else if ( strcasecmp(reco->info.lexema, "para") == 0 ){
            edo = 7;
            struct nodo *recoAux = sintaxisPara(reco, 1);
            if(recoAux == NULL){
                edo = 8;
                sintaxisPrograma(NULL, edo);
            }
            else
                sintaxisPrograma(recoAux, edo);
        }else if(reco != NULL) {
            edo = 8;
            sintaxisPrograma(reco, edo);
        }
    } else {
        //printf("Estado 8\n");
        //printf(reco->info.lexema);
        //if ( strcasecmp(reco->info.nombre, "CorcheteDer") != 0 ){
        if ( reco == NULL ){
            //printf("Error de sintaxis: Se esperaba el simbolo Corchete de cierre ( ] ) en linea %d columna %d", reco->info.numLin, reco->info.numCol);
            printf("Error de sintaxis: Se esperaba el simbolo Corchete de cierre ( ] ) en la ultima linea");
        } else {
            printf("Analisis sintactico correcto, FELICIDADEEEES");
        }
    }
}

void analizadorLexico(struct nodo *reco){
    sintaxisPrograma(reco, 1);
}

int main(int argc, char *argv[]) {
    //setlocale(LC_ALL,"");
    if(argc >= 2){                                              // Verifica que el programa se haya ejecutado con un parametro
        char nombreArchivo[strlen(argv[1])];
        strcpy(nombreArchivo, argv[1]);
        printf("\t\t%s:\n", nombreArchivo);

        FILE *archivo;                                          // Abre el archivo mandado en el parametro y lo guarda en strArchivo
        archivo = fopen(nombreArchivo, "r");
        if(archivo == NULL){
            printf("ERROR de apertura del archivo\n");
            fclose(archivo);
        }
        else{
            int sizeArchivo = tamArchivo(archivo);
            printf("Numero de caracteres en el archivo: %d\n", sizeArchivo);
            printf("---------------------------------\n");
            char strArchivo[sizeArchivo];
            fflush(stdin);
            for(int x = 0; x<sizeArchivo-2; x++){
                char c = fgetc(archivo);
                strArchivo[x] = c;
            }
            fclose(archivo);
            strArchivo[sizeArchivo-1] = EOF;
            printf("%s\n", strArchivo);
            printf("---------------------------------\n");
            leerTokens(strArchivo);

            imprimirLista(raiz);

            analizadorLexico(raiz);
        }
    }
    else{
        printf("Error, no argumentos.");
    }
    //getchar();
    return 0;
}
