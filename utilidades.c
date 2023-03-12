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
        nameC: nombre por defecto para el archivo de carga
        t: valor real de un minuto de simulacion
    Retrona:
        Un numero mayor que 0 si todo fue bien. Un numero menor o igual a 0 si algo salio mal 
*/
int comprobarEntrada(int argc, char *argv[], char *nameC, double *t) {
    FILE * temp;
    /*comprobamos que la entrada tenga mas de 1 argumento*/
    if (argc < 3) {
        printf("Error, too few arguments\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n",argv[0]);
        return -1;
    }

    /* comprobamos que el -s es correcto */
     if (strcmp(argv[1],"-s") != 0){
            printf("Error with the arguments\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[0]);
            return -1;
    }

    if ((temp = fopen(argv[2], "r")) == NULL){
        printf("Error with the argument -s \"%s\"\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[2], argv[0]);
        return -1;
    }
    fclose(temp);

    if (argc == 5) {
        if (strcmp(argv[3],"-c") == 0) { /* si existe comprobamos que el -c es correcto*/
            
            if ((temp = fopen(argv[4], "r")) == NULL){
                printf("Error with the argument -c \"%s\"\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[4], argv[0]);
                return -1;
            }
            fclose(temp);
            memset(nameC,0,MAX_NAME_FILE);
            strcpy(nameC,argv[4]);

        } else if (strcmp(argv[3],"-t") == 0) { /* si existe comprobamos que el -t es correcto*/
            if (atof(argv[4]) <= 0){ /* no puedes tener un tiempo de 0 o negativo*/
                printf("Error with the argument -t \"%s\"\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n",argv[4],argv[0]);
                return -1;
            }

            *t = atof(argv[4]);

            if ((temp = fopen(nameC, "r")) == NULL){
                printf("Error with the file \"%s\" maybe it doesn't exists \n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", nameC, argv[0]);
                return -1;
            }
            fclose(temp);

        } else {
            printf("Error with the arguments\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[0]);
            return -1;
        }
    } else if (argc == 7) {
        if ((strcmp(argv[3],"-c") != 0) || ((temp = fopen(argv[4], "r")) == NULL)){
                printf("Error with the argument -c \n Use: %s -s <file name> [-c <file name>] [-t <num>]\n",argv[0]);
                return -1;
            }
        fclose(temp);
        
        memset(nameC,0,MAX_NAME_FILE);
        strcpy(nameC,argv[4]);
        
        if ((strcmp(argv[5],"-t") != 0) || (atof(argv[6]) <= 0)){ /* no puedes tener un tiempo de 0 o negativo*/
                printf("Error with the argument -t \n Use: %s -s <file name> [-c <file name>] [-t <num>]\n",argv[0]);
                return -1;
            }
            
        *t = atof(argv[6]);
    }

    return 0;
};

/*
    Comprueba si las horas y los minutos estan en el rango valido
    Argumentos:
        horas: un entero que representa las horas
        minutos: un entero que representa los minutos
    Retorna:
        Un numero mayor que 0 si todo fue bien. Un numero menor o igual a 0 si algo salio mal 
*/
int verificarTiempo(int horas, int minutos) {
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
time_t strToTime(char *str) {
    int horas, minutos;
    sscanf(str,"%d:%d",&horas,&minutos);
    if (verificarTiempo(horas,minutos) == 0) {
        printf("Error with the time\n");
        return;
    }
    
    struct tm d = {};
    d.tm_hour = horas;
    d.tm_min = minutos;

    return mktime(&d);
}

/*
 Funcion que cuenta el numero de rutas mediante un archivo
 Argumentos:
    *fp: un apuntador a un archivo
Retorna:
    n: numero de rutas
 */
int get_number_routes(FILE *fp)
{
    int n = 0;
    /* El numero de lineas del archivo indica el numero de rutas restandole menos 1 por la primera fila del archivo */
    char c = getc(fp);
    while (c != EOF)
    {
        if (c == '\n')
            n++;
        c = getc(fp);
    }
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
        fscanf(fp, "%[^,], %[^,], %[^,], %d, %d, %d, %d, %d, %d, %d, %d", code, load_name,travel_time, &g[0], &g[1], &g[2], &g[3], &g[4], &g[5], &g[6], &g[7]);

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


        /* Agregamos esta nueva carga al arreglo */
        arr[i] = new_load;

        /* obtenemos el caracter de la nueva linea */
        if(getc(fp) != '\n'){
            getc(fp);
        }
    }

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
            char *string_hora = malloc(MAX_TRAVEL_TIME_LENGTH);
            while (isdigit(c) || c == ':')
            {
                strncat(string_hora, &c, 1);
                c = getc(fp);
            }
            fscanf(fp, "%d", &capacity);
            agregarServicio(string_hora,capacity,new_itinerary);
        }
        else if (c == '\n')
        {
            /* guardemos el nuevo itinerario en el arreglo de routes_service */
            arr[i] = new_itinerary;
            i++;
        }
        c = getc(fp);
    }
    /* nos falta guardar el ultimo nuevo itinerario en el arreglo */
    arr[i] = new_itinerary;
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
t_carga ** leerCarga(char *carga, int *numeroRutas)  {
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
    t_carga **loads=malloc(sizeof(t_carga*)*n);

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
itinerario ** leerServicio(char *servicio, int *numeroRutas)  {
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
    itinerario **routes_service = malloc(sizeof(itinerario*)*n);
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
void imprimirHora(time_t *hora) {
    char time[MAX_TRAVEL_TIME_LENGTH];
    strftime(time,MAX_TRAVEL_TIME_LENGTH,"%H:%M",localtime(hora));
    printf("%s\n",time);
}
