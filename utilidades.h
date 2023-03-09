#ifndef UTILIDADES_LIB
    #include <time.h>
    #include <stdio.h>
    #include "carga.h"
    
    #define UTILIDADES_LIB
    #define MAX_NAME 20 /*El tamaño maximo de un nombre de archivo*/

    int comprobarEntrada(int argc, char *argv[], char *nameC, double *t);
    time_t strToTime(char *str);

    /*
    Funcion que cuenta el numero de rutas mediante un archivo
        Argumentos:
        *fp: un apuntador a un archivo
    Retorna:
    n: numero de rutas
    */
    int get_number_routes(FILE *fp);

    /*
    Funcion que lee el archivo de carga y construye el arreglo donde se guarda estas cargas
    Argumentos:
        n: un entero que indica el numero de rutas
        *fp: un apuntador a un archivo
        arr: un arreglo de tipo t_carga
    */
    void build_loads(int n, FILE *fp, t_carga *arr[]);

#endif
