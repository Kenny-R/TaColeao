#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "utilidades.h"

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
int comprobarEntrada(int argc, char *argv[], char *nameC, char *nameS, double *t)
{
    FILE *temp;
    /*comprobamos que la entrada tenga mas de 1 argumento*/
    if (argc < 3)
    {
        printf("Error, too few arguments\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[0]);
        return -1;
    }

    /* comprobamos que el -s es correcto */
    if (strcmp(argv[1], "-s") != 0)
    {
        printf("Error with the arguments\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[0]);
        return -1;
    }
    if ((temp = fopen(argv[2], "r")) == NULL)
    {
        printf("Error with the argument -s \"%s\"\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[2], argv[0]);
        return -1;
    }
    
    fclose(temp);
    memset(nameS, 0, MAX_NAME_FILE);
    strcpy(nameS, argv[2]);

    if (argc == 5)
    {
        if (strcmp(argv[3], "-c") == 0)
        { /* si existe comprobamos que el -c es correcto*/

            if ((temp = fopen(argv[4], "r")) == NULL)
            {
                printf("Error with the argument -c \"%s\"\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[4], argv[0]);
                return -1;
            }
            fclose(temp);
            memset(nameC, 0, MAX_NAME_FILE);
            strcpy(nameC, argv[4]);
        }
        else if (strcmp(argv[3], "-t") == 0)
        { /* si existe comprobamos que el -t es correcto*/
            if (atof(argv[4]) <= 0)
            { /* no puedes tener un tiempo de 0 o negativo*/
                printf("Error with the argument -t \"%s\"\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[4], argv[0]);
                return -1;
            }

            *t = atof(argv[4]);

            if ((temp = fopen(nameC, "r")) == NULL)
            {
                printf("Error with the file \"%s\" maybe it doesn't exists \n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", nameC, argv[0]);
                return -1;
            }
            fclose(temp);
        }
        else
        {
            printf("Error with the arguments\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[0]);
            return -1;
        }
    }
    else if (argc == 7)
    {
        if ((strcmp(argv[3], "-c") != 0) || ((temp = fopen(argv[4], "r")) == NULL))
        {
            printf("Error with the argument -c \n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[0]);
            return -1;
        }
        fclose(temp);

        memset(nameC, 0, MAX_NAME_FILE);
        strcpy(nameC, argv[4]);

        if ((strcmp(argv[5], "-t") != 0) || (atof(argv[6]) <= 0))
        { /* no puedes tener un tiempo de 0 o negativo*/
            printf("Error with the argument -t \n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[0]);
            return -1;
        }

        *t = atof(argv[6]);
    }

    return 1;
};

/*
    Comprueba si las horas y los minutos estan en el rango valido
    Argumentos:
        horas: un entero que representa las horas
        minutos: un entero que representa los minutos
    Retorna:
        Un numero mayor que 0 si todo fue bien. Un numero menor o igual a 0 si algo salio mal
*/
int verificarTiempo(int horas, int minutos)
{
    if (horas > 23 || horas < 0)
        return 0;
    if (minutos > 59 || minutos < 0)
        return 0;
    return 1;
}

/*
    Transforma un string, que representa una hora con el formato HH:MM, en un time_t
    Argumentos:
        str: El string con la hora en formato HH:MM
    Retorna:
        un time_t donde todos los datos son los por defecto menos las horas y los minutos
*/
time_t strToTime(char *str)
{
    int horas, minutos;
    sscanf(str, "%d:%d", &horas, &minutos);
    if (verificarTiempo(horas, minutos) == 0)
    {
        printf("Error with the time\n");
        return;
    }

    time_t s;
    time(&s);
    struct tm *d;
    d = localtime(&s); 
    d->tm_hour = horas;
    d->tm_min = minutos;
    d->tm_sec = 0;
    return mktime(d);
}

/*
 Funcion que cuenta el numero de rutas mediante un archivo
 Argumentos:
    fp: un apuntador a un archivo
Retorna:
    n: numero de rutas
 */
int get_number_routes(FILE *fp)
{
    int n = 0;
    /* El numero de lineas del archivo indica el numero de rutas */
    char c = getc(fp);
    int hay_linea_vacia = 0;
    while (c != EOF)
    {
        if (c == '\n')
        {
            n++;
            c = getc(fp);
            if (c == '\n' || c == EOF) {
                hay_linea_vacia = 1;
                break;
            }
        }
        c = getc(fp);
    }
    
    if (hay_linea_vacia == 1)
        n--;

    return n;
}

/*
 Funcion que lee el archivo de carga y construye el arreglo donde se guarda estas cargas
 Argumentos:
    n: un entero que indica el numero de rutas
    *fp: un apuntador a un archivo
    arr: un arreglo de tipo t_carga
 */
void build_loads(int n, FILE *fp, t_carga *arr[])
{
    /* leemos la linea del archivo de carga sin realizar nada */
    char c;
    while (c != '\n')
        c = getc(fp);

    /* Comenzamos a leer las siguientes lineas del archivo */
    t_carga *new_load;
    char load_name[MAX_LOAD_NAME_LENGTH], travel_time[MAX_TRAVEL_TIME_LENGTH], code[MAX_CODE_LENGTH];
    int g[8];
    int i;
    for (i = 0; i < n; ++i)
    {
        fscanf(fp, "%[^,], %[^,], %[^,], %d, %d, %d, %d, %d, %d, %d, %d", code, load_name, travel_time, &g[0], &g[1], &g[2], &g[3], &g[4], &g[5], &g[6], &g[7]);

        /* Creamos la carga con los datos que nos dieron */
        new_load = crearCarga(code, load_name, travel_time);

        /* Agregamos los 8 grupos */
        agregarGrupo("06:00", g[0], new_load);
        agregarGrupo("07:00", g[1], new_load);
        agregarGrupo("08:00", g[2], new_load);
        agregarGrupo("09:00", g[3], new_load);
        agregarGrupo("10:00", g[4], new_load);
        agregarGrupo("11:00", g[5], new_load);
        agregarGrupo("12:00", g[6], new_load);
        agregarGrupo("13:00", g[7], new_load);

        /* agregamos la cantidad total de pasajeros de la ruta */
        new_load->pasajeros += (g[0] + g[1] + g[2] + g[3] + g[4] + g[5] + g[6] + g[7]);
        
        /* Agregamos esta nueva carga al arreglo */
        arr[i] = new_load;

        /* obtenemos el caracter de la nueva linea */
        if (getc(fp) != '\n')
        {
            getc(fp);
        }
    }
    return;
}

/*
 Funcion que lee el archivo de servicio y construye el arreglo donde se guarda los servicios de las rutas
 Argumentos:
    n: un entero que indica el numero de rutas
    *fp: un apuntador a un archivo
    arr: un arreglo de tipo itinerario
 */
void build_services(int n, FILE *fp, itinerario *arr[])
{
    int capacity, i = 0;
    itinerario *new_itinerary;
    char c = getc(fp);
    while (c != EOF)
    {
        if (isalpha(c))
        {
            /* Leemos el codigo de la parada */
            char codigo[MAX_CODE_LENGTH] = "";
            while (isalpha(c))
            {
                strncat(codigo, &c, 1);
                c = getc(fp);
            }
            new_itinerary = crearItinerario(codigo);
        }
        else if (isdigit(c))
        {
            /* Leemos la hora en que sale el autobus y su capacidad */
            /*(Tiene el mismo tamaño que un travel time)*/
            char *string_hora = calloc(MAX_TRAVEL_TIME_LENGTH, sizeof(char));
            if (string_hora == NULL)
            {
                printf("No hay suficiente memoria disponible\n");
                exit(1);
            }
            while (isdigit(c) || c == ':')
            {
                strncat(string_hora, &c, 1);
                c = getc(fp);
            }
            fscanf(fp, "%d", &capacity);
            agregarServicio(string_hora, capacity, new_itinerary);
            free(string_hora);
        }
        else if (c == '\n')
        {
            /* guardemos el nuevo itinerario en el arreglo de routes_service */
            arr[i] = new_itinerary;
            i++;
            if (i == n)
                break;
        }
        c = getc(fp);
    }
    /* nos falta guardar el ultimo nuevo itinerario en el arreglo */
    arr[i] = new_itinerary;
    return;
}

/*
    Dado el nombre de un archivo que representa una representacion de la carga de una parada,
    rellena un array con todos los datos del archivo.
    Argumento:
        carga: El nombre(o direccion) del archivo a leer
        numeroRutas: un apuntador a un entero donde se almacenara el numero de rutas del archivo
    Retorna:
        Un arreglo de structs t_carga que representa la carga de cada parada
*/
t_carga **leerCarga(char *carga, int *numeroRutas)
{
    /* numero de rutas */
    int n = 0;

    /* primero, abrimos y Leemos el archivo de carga para obtener el numero de rutas */
    char file_name[MAX_NAME_FILE];
    strcpy(file_name, carga);
    FILE *load_file = fopen(file_name, "r");
    /* Verifiquemos si se pudo abrir correctamente el archivo */
    if (load_file == NULL)
    {
        printf("The file could not be open\n");
        return NULL;
    }
    n = get_number_routes(load_file);

    *numeroRutas = n;

    /* Creamos un arreglo para las cargas */
    t_carga **loads = malloc(sizeof(t_carga *) * n);
    if (loads == NULL) {
        printf("No hay suficiente memoria disponible\n");
        exit(1);
    }

    /* Volvemos a leer desde el comienzo del archivo */
    rewind(load_file);
    /* Y construiremos el arreglo de las cargas */
    build_loads(n, load_file, loads);
    /* Cerramos el archivo de carga */
    fclose(load_file);

    return loads;
}

/*
    Dado el nombre de un archivo que representa una representacion del servicio de una parada,
    rellena un array con todos los datos del archivo.
    Argumento:
        servicio: El nombre(o direccion) del archivo a leer
        numeroRutas: un apuntador a un entero donde se almacenara el numero de rutas del archivo
    Retorna:
        Un arreglo de structs itinerario que representa el servicio para cada parada
*/
itinerario **leerServicio(char *servicio, int *numeroRutas)
{
    /* numero de rutas */
    int n = 0;

    /* primero, abrimos y Leemos el archivo de carga para obtener el numero de rutas */
    char file_name[MAX_NAME_FILE];

    strcpy(file_name, servicio);
    FILE *service_file = fopen(file_name, "r");

    /* Verifiquemos si se pudo abrir correctamente el archivo */
    if (service_file == NULL)
    {
        printf("The file could not be open\n");
        return NULL;
    }

    n = get_number_routes(service_file) + 1;
    *numeroRutas = n;

    rewind(service_file);
    /* Creamos un arreglo para guardar los servicios de cada parada */
    itinerario **routes_service = malloc(sizeof(itinerario *) * n);

    if (routes_service == NULL) {
        printf("No hay suficiente memoria disponible\n");
        exit(1);
    }

    build_services(n, service_file, routes_service);

    /* Cerramos el archivo */
    fclose(service_file);
    return routes_service;
}

/*
    Dado un apuntador a un time_t imprime la informacion en
    formato HH:MM. Pone un \n al final de la impresion siempre
    Argumento:
        hora: el time_t con la infromacion
*/
void imprimirHora(time_t *hora)
{
    char time[MAX_TRAVEL_TIME_LENGTH];
    strftime(time, MAX_TRAVEL_TIME_LENGTH, "%H:%M", localtime(hora));
    printf("%s\n", time);
}

/*
    Envia un mensaje a traves de un pipe
    Argumentos:
        fd: file descriptor para el pipe de escritura
        origen: un string que indica quien es el que escribe el mensaje
        destino: un string que indica a quien se le va a enviar el mensaje
        hora: la hora en que se envia el mensaje
        mensaje: el texto que se va a enviar por el pipe
*/
void enviarMensaje(int *fd, char origen[], char destino[], time_t *hora, char mensaje[])
{

    if (strlen(origen) > MAX_ORIGEN_LENGTH ||
        strlen(destino) > MAX_DESTINO_LENGTH ||
        strlen(mensaje) > MAX_MENSAJE_LENGTH)
    {
        printf("No se puede enviar un mensaje tan grande\n");
        return;
    }

    /*Creamos el str de la hora*/
    char time[6];
    strftime(time, 6, "%H:%M", localtime(hora));

    /*calculamos parte del tamaño del mensaje*/
    int n = strlen(origen) + 1 + strlen(destino) + 1 + strlen(time) + 1 + strlen(mensaje) + 1;

    /*tansfromamos el entro n en un string*/
    char largo[sizeof(int) * 8 + 1];
    sprintf(largo, "%d", n);

    /*Calculamos el tamaño total del mensaje*/
    int m = n + strlen(largo) + 1;

    /*creamos el mensaje final*/
    char mensajeFinal[m];
    sprintf(mensajeFinal, "%d|%s|%s|%s|%s", n, origen, destino, time, mensaje);
    /*Enviamos el mensaje*/
    write(*fd, mensajeFinal, m);
}

/*
    Lee un mensaje a traves de un pipe
    Argumentos:
        fd: file descriptor para el pipe de lectura
        origen: un string que indica quien es el que escribió el mensaje
        destino: un string que indica para quien fue el mensaje
        hora: la hora en que se envió el mensaje
        mensaje: el texto que se envió por el pipe
*/
void leerMensaje(int *fd, char origen[], char destino[], time_t *hora, char mensaje[])
{
    /*leemos la primera parte del mensaje hasta que encontremos un "|" */
    /* esto es para encontrar el largo del mensaje */
    char c[2];
    char largo[sizeof(int) * 8 + 1] = "";
    while (TRUE)
    {
        read(*fd, c, 1);
        c[1] = '\0';
        if (c[0] != '|')
        {
            strcat(largo, c);
        }
        else
        {
            break;
        }
    }

    /* convertirmos el string en un entero */
    int n = atoi(largo);
    /* leemos el mensaje con la funcion read */
    char contenido[n];
    read(*fd, contenido, n);
    char time[6];
    sscanf(contenido, "%[^|]|%[^|]|%[^|]|%[^|]", origen, destino, time, mensaje);
    *hora = strToTime(time);
}

/*
    Dado un struct t_carga y la hora en time_t, va a calcular la cantidad de personas esperando en una ruta
    Argumento:
        infoCarga: un apuntador a la carga de una ruta
        hora: hora actual de la simulacion
    Retorna:
        el numero de personas esperando en la ruta
*/
int numeroDePersonasEnEspera(t_carga *infoCarga, time_t hora)
{
    nodo *nodoActual = infoCarga->grupos->siguiente;
    t_grupo *contenido = (t_grupo *)(nodoActual->contenido);
    int nro = 0;
    while (contenido != NULL)
    {
        if (difftime(hora, contenido->hora) >= 0)
        {
            nro = nro + contenido->cantidad;
            nodoActual = nodoActual->siguiente;
            contenido = (t_grupo *)(nodoActual->contenido);
        }
        else
        {
            break;
        }
    }

    return nro;
}

/*
    Codifica el mensaje que se va a enviar a traves del pipe a un formato:
    "I,nP,signo_1.avance_1,...,signo_n.avance_n\n" donde 
    nP es el número de personas esperando en una parada,
    signo_i puede ser < o > dependiendo de la dirección del autobús y 
    avance_i es el porcentaje de avance (del 0% al 100%).
    Argumento:
        arrAvances: un apuntador a un arreglo de struct avance
        nombreRuta: un string que contiene el nombre de la ruta
        nroPersonasEnEspera: el numero de personas esperando en la ruta
        serviciosArrancados: cantidad de autobuses que se encuentra activos
        largoTotal: largo del mensaje
    Retorna:
        el mensaje codificado
*/
char *codficarInformacion(struct avance *arrAvances, char *nombreRuta, int nroPersonasEnEspera, int serviciosArrancados, int largoTotal)
{
    char *resultado = calloc(largoTotal, sizeof(char));
    strcat(resultado, "I,");
    char buffer[sizeof(int) * 8 + 1];
    sprintf(buffer, "%d", nroPersonasEnEspera);
    strcat(resultado, buffer);
    int i = 0;
    int hayProgreso =0;

    for (; i < serviciosArrancados; i++)
    {
        if(arrAvances[i].ida == -2)
            continue;

        if (arrAvances[i].ida != 2)
        {
            hayProgreso = 1;
            strcat(resultado, ",");
            if (arrAvances[i].ida == 1)
            {
                strcat(resultado, ">");
                sprintf(buffer, "%d", (arrAvances[i].arrPorcentajes[arrAvances[i].pos]) / 10);
                strcat(resultado, buffer);
            }
            else if (arrAvances[i].ida == 0)
            {
                strcat(resultado, "<");
                sprintf(buffer, "%d", (arrAvances[i].arrPorcentajes[arrAvances[i].pos]) / 10);
                strcat(resultado, buffer);
            }
            else
            {
                strcat(resultado, ">10");
            }
        } else {
            hayProgreso = 1;
            strcat(resultado, ",<10");
            arrAvances[i].ida = -2;

        }
    }
    
    strcat(resultado, "\n");
    if(hayProgreso == 0)
        return "ta vacio we\n";
    return resultado;
}

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
void imprimirMsg(char codParada[], char msg[]) {
    int j = 2;
    int msgValido = 0;

    /* se asegura que el mensaje es válido */
    while (msg[j] != '\0') {
        if (msg[j++] == ',') {
            msgValido = 1;
            break;
        }    
    }

    int i = 2;
    int cont = 0;
    char prog;

    /* se imprime el código de la parada y el número de personas que esperan */
    printf("%s: ", codParada);
    while (msg[i] != ',')
    {
        printf("%c", msg[i++]);
    }

    i++;

    /* se decodifica e imprime el progreso de cada autobús activo de la parada */
    while (1)
    {
        /* se guarda el signo > o < */
        prog = msg[i++];
        cont = 0;
        /* se determina el porcentaje de avance al leer los caracteres correspondientes */
        while (msg[i] != ',' && msg[i] != '\n')
        {
            cont *= 10;
            cont += msg[i++] - '0';
        }
        
        /* impresión del porcentaje */
        printf(" [");
        for (j = 0; j < cont; j++)
        {
            printf("%c", prog);
        }

        for (j = cont; j < 10; j++)
        {
            printf(" ");
        }
        
        printf("]");
        
        /* se determina si ya terminó el mensaje */
        if (msg[i++] == '\n') break;
    }
    printf("\n");
}
