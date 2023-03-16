#ifndef UTILIDADES_LIB
    #include <time.h>
    #include <stdio.h>
    #include "carga.h"
    #include "itinerario.h"
    
    #define UTILIDADES_LIB
    #define MAX_NAME_FILE 30 /*El tamaño maximo de un nombre de archivo*/
    #define MAX_LOAD_NAME_LENGTH 30
    #define MAX_TRAVEL_TIME_LENGTH 6
    #define MAX_CODE_LENGTH 4
    
    /* 
        Comprueba los datos de entrada si todo fue correcto un numero distinto mayor a 0 si algo fallo devuelve un numero menor
        o igual a 0
        Argumentos:
            argc: contador de argumentos en argv
            argv: arreglo de argumentos
            nameC: string donde se almacenara el nombre del archivo de carga
            nameS: string donde se almacenara el nombre del archivo de servicio
            t: Valor real donde se almacenara la correspondecia de un minuto de 
            simulacion en segundos reales
        Retrona:
            Un numero mayor que 0 si todo fue bien. Un numero menor o igual a 0 si algo salio mal 
    */
    int comprobarEntrada(int argc, char *argv[], char *nameC, char *nameS, double *t) ;
    
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
        Dado el nombre de un archivo que representa una representacion de la carga de una parada, 
        rellena un array con todos los datos del archivo.
        Argumento:
            carga: El nombre(o direccion) del archivo a leer
            numeroRutas: un apuntador a un entero donde se almacenara el numero de rutas del archivo
        Retorna:
            Un arreglo de structs t_carga que representa la carga de cada parada 
    */
    t_carga ** leerCarga(char *carga, int *numeroRutas);
    
    /*
        Dado el nombre de un archivo que representa una representacion del servicio de una parada, 
        rellena un array con todos los datos del archivo.
        Argumento:
            servicio: El nombre(o direccion) del archivo a leer
            numeroRutas: un apuntador a un entero donde se almacenara el numero de rutas del archivo
        Retorna:
            Un arreglo de structs itinerario que representa el servicio para cada parada 
    */
    itinerario ** leerServicio(char *servicio, int *numeroRutas);

    /*
        Dado un apuntador a un time_t imprime la informacion en 
        formato HH:MM. Pone un \n al final de la impresion siempre
        Argumento:
            hora: el time_t con la infromacion
    */
    void imprimirHora(time_t *hora);

    double segToSmin(double t);

#endif
