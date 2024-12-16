#include <stdio.h>
#include <string.h>

// Cabeceras de módulos
#include "Lexico/DefinicionLexema.h" // Módulo con estructuras y ENUM de los Tokens
#include "Lexico/DefinicionLexico.h" // Módulo con funciones de análisis léxico
#include "Sintactico/DefinicionSintactico.h" // Módulo con funciones de análisis sintáctico

/**
 * @brief Realiza el análisis léxico a partir de un nodo de la lista.
 *
 * Esta función llama al módulo de análisis sintáctico para procesar la estructura
 * del programa basado en los nodos generados durante el análisis léxico.
 *
 * @param reco Puntero al nodo inicial de la lista enlazada de tokens generada.
 */
void analizadorLexico(struct nodo *reco) {
    sintaxisPrograma(reco, 1);
}

int main(const int argc, char *argv[]) {
    if (argc >= 2) {

        char nombreArchivo[strlen(argv[1])];
        strcpy(nombreArchivo, argv[1]);
        printf("\t\t%s:\n", nombreArchivo);

        FILE *archivo = fopen(nombreArchivo, "r");
        if (archivo == NULL) {
            printf("ERROR :: No se pudo abrir el archivo.\n");
            fclose(archivo);
        } else {
            // Obtención del tamaño del archivo
            int sizeArchivo = tamArchivo(archivo);
            printf("Numero de caracteres en el archivo: %d\n", sizeArchivo);
            printf("---------------------------------\n");

            char strArchivo[sizeArchivo];
            fflush(stdin);
            for (int x = 0; x < sizeArchivo - 2; x++) {
                const char c = fgetc(archivo);
                strArchivo[x] = c;
            }
            fclose(archivo);
            strArchivo[sizeArchivo - 1] = EOF;

            printf("%s\n", strArchivo);
            printf("---------------------------------\n");

            // Léxico - Primera revisión
            leerTokens(strArchivo);
            imprimirLista(raiz);

            // Sintáctico usando el analizador léxico
            analizadorLexico(raiz);
        }
    } else {
        printf("ERROR :: No se paso como argumento el nombre del archivo.\n");
    }
    return 0;
}