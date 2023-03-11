#ifndef UTILIDADES_LIB
    #include <time.h>
    #include <stdio.h>
    #include "carga.h"
    #include "itinerario.h"
    
    #define UTILIDADES_LIB
    #define MAX_NAME 20 /*El tamaño maximo de un nombre de archivo*/
    #define MAX_LOAD_NAME_LENGTH 30
    #define MAX_TRAVEL_TIME_LENGTH 6
    #define MAX_CODE_LENGTH 4
    
    /* 
        Comprueba los datos de entrada si todo fue correcto un numero distinto mayor a 0 si algo fallo devuelve un numero menor
        o igual a 0
        Argumentos:
            argc: contador de argumentos en argv
            argv: arreglo de argumentos
            nameC: nombre por defecto para el archivo de carga
            t: valor real de un minuto de simulacion
        Retrona:
            Un numero mayor que 0 si todo fue bien. Un numero menor o igual a 0 si algo salio mal 
    */
    int comprobarEntrada(int argc, char *argv[], char *nameC, double *t);
    
    /*
        Comprueba si las horas y los minutos estan en el rango valido
        Argumentos:
            horas: un entero que representa las horas
            minutos: un entero que representa los minutos
        Retorna:
            Un numero mayor que 0 si todo fue bien. Un numero menor o igual a 0 si algo salio mal 
    */
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

    /*
    Funcion que lee el archivo de servicio y construye el arreglo donde se guarda los servicios de las rutas
    Argumentos:
        n: un entero que indica el numero de rutas
        *fp: un apuntador a un archivo
    arr: un arreglo de tipo itinerario
    */
    void build_services(int n, FILE *fp, itinerario *arr[]);
    
    /*
        Dado dos nombres y dos arreglos de cargas e itinerarios, lee los archivos con esos nombres y rellena
        su arreglo correspondiente.
        Argumentos:
            carga: Nombre del archivo de caracterizacion de carga
            servicio: Nombre del archivo de caracterizacion de servicio
            loads: La direccion donde se almacenara el arreglo con todas las cargas en el archivo carga
            routes_service: La direccion donde se almacenara el arreglo con todos los servicios en el archivo servicio
    */
    void leerDatos(char *carga, char *servicio, t_carga ***loads, itinerario ***routes_service);
#endif
