#include "utilidades.h"
#include "carga.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
    Dado un codigo, un nombre y un tiempo de recorrido crea una nueva carga
    Argumentos:
        cod: La direccion donde se encuentra el codigo que se va a almacenar
        name: La direccion donde se encuentra el nombre que se va a almacenar
        tiempoRecorr: La direccion donde se encuentra el tiempo de recorrido
                      que se va a almacenar
    Retorna:
        un struct t_carga que almacena toda la informacion relaciondad a una
        carga
*/
t_carga *crearCarga(char *cod, char *name, char *tiempoRecorr){
    t_carga *nuevaCarga = malloc(sizeof(t_carga));
    
    if (nuevaCarga == NULL) {
        printf("No hay suficiente memoria disponible\n");
        exit(1);
    }

    nuevaCarga -> cod = malloc(MAX_CODE_LENGTH);
    if (nuevaCarga->cod == NULL) {
        printf("No hay suficiente memoria disponible\n");
        exit(1);
    }
    strcpy(nuevaCarga->cod,cod);
    
    nuevaCarga -> name = malloc(MAX_LOAD_NAME_LENGTH);
    if (nuevaCarga->name == NULL) {
        printf("No hay suficiente memoria disponible\n");
        exit(1);
    }

    strcpy(nuevaCarga->name,name);

    nuevaCarga -> recorr = strToTime(tiempoRecorr);
    nuevaCarga -> pasajeros = 0;
    nuevaCarga -> grupos = crearListaEnlazada();

    return nuevaCarga;
}

/*
    Dado una hora y una cantidad las agurapa en un struct que representa un grupo
    Argumentos:
        hora: La hora a la que llego el grupo
        cantidad: La cantidad de individuos que componen al grupo
    Retorna:
        un struct t_grupo que representa a un grupo de individuos
*/
t_grupo *crearGrupo(char *hora, int cantidad) {
    t_grupo *nuevoGrupo = malloc(sizeof(t_grupo));
    if (nuevoGrupo == NULL) {
        printf("No hay suficiente memoria disponible\n");
        exit(1);
    }

    nuevoGrupo->cantidad = cantidad;
    nuevoGrupo -> hora = strToTime(hora);

    return nuevoGrupo;
}

/*
    Dado una hora, una cantidad y una carga crea un grupo y lo agrega al final de la carga
    Argumentos:
        hora: La hora a la que llego el grupo
        cantidad: La cantidad de individuos que componen al grupo
        carga: La carga donde se agregara el nuevo grupo
*/
void agregarGrupo(char *hora, int cantidad,t_carga *carga) {
    t_grupo *nuevoGrupo = malloc(sizeof(t_grupo));
    if (nuevoGrupo == NULL) {
        printf("No hay suficiente memoria disponible\n");
        exit(1);
    }
    nuevoGrupo -> hora = strToTime(hora);
    nuevoGrupo -> cantidad = cantidad;

    anadirNodoAlFinal(nuevoGrupo,carga->grupos);
}

/*
    Dada una cantidad se intena reducir lo mas posible esta cantidad usando el primer grupo 
    de la cola de la carga
    Argumentos:
        cantidad: la direccion donde se encuentra la cantidad que se quiere intentar reducir
        nroPuntuales: la dirección donde se encuentra el número de personas que han llegado
                    puntual hasta los momentos
        espera: entero que representa el tiempo en minutos que aun queda por esperar para que
                el autobús empiece su recorrido hacia la USB
        recorr: entero que representa el tiempo en minutos del recorrido del autobús
        carga: Carga donde se reducira la cantidad 
        hora_actual: la hora actual de la simulacion
    Retorna:
        un time_t con la hora del grupo que se uso para reducir la cantidad solicitada (no 
        necesariamente se reduce toda la cantidad que se requiere puede reducirse menos pero
        se actualiza el contenido de cantidad)
*/
time_t reducirCarga(int *cantidad, int *nroPuntuales, int espera, int recorr, t_carga *carga, time_t hora_actual)
{   
    int reduc = 0;
    t_grupo *contenido;
    contenido = (t_grupo *)(carga->grupos->siguiente->contenido);
    if (contenido == NULL)
        return;
    time_t tiempo = contenido->hora;
    if (tiempo <= hora_actual)
    {
        if ((contenido->cantidad - *cantidad) <= 0)
        { /*la cantidad solicitada es mayor a la cantidad de persona en espera*/
            reduc += contenido->cantidad;
            *cantidad = *cantidad - reduc;
            eliminarNodo(carga->grupos->siguiente);
        }
        else if ((contenido->cantidad - *cantidad) > 0)
        {
            reduc += *cantidad;
            contenido->cantidad = contenido->cantidad - *cantidad;
            *cantidad = 0;
        }

        /* se determina si la carga reducida llegará temprano, y si es así se suma al número de personas que llegaron temprano */
        if (reduc && difftime(hora_actual, tiempo) + recorr*60 + espera*60 <= 5460)  
            *nroPuntuales += reduc;

    }
    return tiempo;
}


/*
    dada una carga libera la memoria utilizada por esta carga
    Argumentos:
        carga: La carga que se va a liberar 
*/
void eliminarCargar(t_carga *carga) {
    eliminarLista(carga->grupos);
    free(carga->cod);
    free(carga->name);
    free(carga);
}

/*
    dado un codigo y un arreglo de cargas, busca en este arreglo 
    la carga que tenga ese codigo. Si no hay un arreglo con ese 
    codigo devuelve NULL
    Argumento:
        cod: El codigo a buscar
        arr: El arreglo de cargas
        len: El largo del arreglo
    Retorna:
        la direccion de la carga con el codigo solicitado, o null si 
        no se encontro la carga.
*/
t_carga * buscarCarga(char *cod, t_carga **arr, int len){
    t_carga *buscado = NULL;
    int i;
    for (i = 0; i < len; i++ ) {
        if (strcmp(cod,arr[i]->cod) == 0) {
            buscado = arr[i];
            break;
        }
    }
    return buscado;
}
