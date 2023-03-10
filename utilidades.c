#include "utilidades.h"
#include "itinerario.h"
#include "carga.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* Comprueba los datos de entrada si todo fue correcto un numero distinto mayor a 0 si algo fallo devuelve un numero menor
o igual a 0*/
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
            memset(nameC,0,MAX_NAME);
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
        
        memset(nameC,0,MAX_NAME);
        strcpy(nameC,argv[4]);
        
        if ((strcmp(argv[5],"-t") != 0) || (atof(argv[6]) <= 0)){ /* no puedes tener un tiempo de 0 o negativo*/
                printf("Error with the argument -t \n Use: %s -s <file name> [-c <file name>] [-t <num>]\n",argv[0]);
                return -1;
            }
            
        *t = atof(argv[6]);
    }

    return 0;
};

int verificarTiempo(int horas, int minutos) {
    if (horas > 23 || horas < 0) 
        return 0;
    if (minutos > 59 || minutos < 0)
        return 0;
    return 1;
    
}

time_t strToTime(char *str) {
    int horas, minutos;
    sscanf(str,"%d:%d",&horas,&minutos);
    if (verificarTiempo(horas,minutos) == 0) {
        printf("Error with the time");
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
    char load_name[30], travel_time[6], code[4], minutes[3];
    int g[8], h, m;
    int i;
    for (i = 0; i < n; ++i)
    {
        fscanf(fp, "%[^,], %[^,],%d:%d,%d,%d,%d,%d,%d,%d,%d,%d", code, load_name, &h, &m, &g[0], &g[1], &g[2], &g[3], &g[4], &g[5], &g[6], &g[7]);

        /* tiempo de recorrido en string */
        sprintf(travel_time, "%d", h);
        strcat(travel_time, ":");
        if (m < 10)
            strcat(travel_time, "0");
        sprintf(minutes, "%d", m);
        strcat(travel_time, minutes);

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
        c = getc(fp);
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
            char codigo[4] = "";
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
            char string_hora[6] = "";
            while (isdigit(c) || c == ':')
            {
                strncat(string_hora, &c, 1);
                c = getc(fp);
            }
            fscanf(fp, "%d", &capacity);
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

