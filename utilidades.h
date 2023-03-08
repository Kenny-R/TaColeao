#ifndef UTILIDADES_LIB
    #include <time.h>
    
    #define UTILIDADES_LIB
    #define MAX_NAME 20 /*El tamaño maximo de un nombre de archivo*/

    int comprobarEntrada(int argc, char *argv[], char *nameC, double *t);
    time_t strToTime(char *str);
#endif
