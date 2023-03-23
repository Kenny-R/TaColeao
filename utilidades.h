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

    /* Para pasarle como argumento a la funcion de los hilos */
    struct avance
    {
        int *asientos;             /* asientos disponibles en el autobus */
        t_carga *cargaParada;      /* lista de las cargas de la parada */
        int *arrPorcentajes; /* un arreglo que indica el porcentaje de los autobuses */
        int ida; /* 1 indica que el autobus va de la uni->parada, 0 si indica que va de parada->uni, -1 indica que esta subiendo los pasajeros y 2 si ya termino */
        int pos; /* posicion del servicio dentro del arreglo porcentajes  */
        time_t tiempoRecorr; /* tiempo de recorrido del autobus */ 
        time_t *horaActual; /* un apuntador a la hora actual de la simulacion */
        time_t horaPartida; /* hora en que parte el autobus hacia la parada */
        int *go; /* un arreglo que indica si es el momento en que el hilo actualice su estado */
        int *servicios_terminados; /* numero de servicios terminados */
    };
    
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

    /*
        Envia un mensaje a traves de un pipe
        Argumentos:
            fd: file descriptor para el pipe de escritura
            origen: un string que indica quien es el que escribe el mensaje
            destino: un string que indica a quien se le va a enviar el mensaje
            *hora: un apuntador a la hora en que se envia el mensaje
            mensaje: el texto que se va a enviar por el pipe
    */
    void enviarMensaje(int *fd, char origen[], char destino[], time_t *hora, char mensaje[]);

    /*
        Lee un mensaje a traves de un pipe
        Argumentos:
            *fd: file descriptor para el pipe de lectura
            origen: un string que indica quien es el que escribió el mensaje
            destino: un string que indica para quien fue el mensaje
            *hora: un apuntador a la hora en que se envió el mensaje
            mensaje: el texto que se envió por el pipe
    */
    void leerMensaje(int *fd, char origen[], char destino[], time_t *hora, char mensaje[]);

    /*
        Dado un struct t_carga y la hora en time_t, va a calcular la cantidad de personas esperando en una ruta
        Argumento:
            *infoCarga: un apuntador a la carga de una ruta
            hora: hora actual de la simulacion
        Retorna:
            el numero de personas esperando en la ruta
    */
    int numeroDePersonasEnEspera(t_carga *infoCarga, time_t hora);

    /*
        Codifica el mensaje que se va a enviar a traves del pipe a un formato:
        "I,nP,signo_1.avance_1,...,signo_n.avance_n\n" donde 
        nP es el número de personas esperando en una parada,
        signo_i puede ser < o > dependiendo de la dirección del autobús y 
        avance_i es el porcentaje de avance (del 0% al 100%).
        Argumento:
            *arrAvances: un apuntador a un arreglo de struct avance
            *nombreRuta: un string que contiene el nombre de la ruta
            nroPersonasEnEspera: el numero de personas esperando en la ruta
            serviciosArrancados: cantidad de autobuses que se encuentra activos
            largoTotal: largo del mensaje
        Retorna:
            el mensaje codificado
    */
    char *codficarInformacion(struct avance *arrAvances, char *nombreRuta, int nroPersonasEnEspera, int serviciosArrancados, int largoTotal);

    /*
        Función que decodifica e imprime dado un mensaje de actualización de estado codificado dado el formato:
        "I,nP,signo_1.avance_1,...,signo_n.avance_n\n" donde 
        nP es el número de personas esperando en una parada,
        signo_i puede ser < o > dependiendo de la dirección del autobús y 
        avance_i es el porcentaje de avance (del 0% al 100%).
        Argumentos:
            codParada: arreglo de caracteres que contiene el código de la parada.
            msg: arreglo de caracteres que contiene el mensaje de actualización de estado codificado.
    */
    void imprimirMsg(char codParada[], char msg[]);

#endif
