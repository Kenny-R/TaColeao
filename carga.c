#include "utilidades.h"
#include "carga.h"
#include <stdio.h>
#include <stdlib.h>

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
    nuevaCarga -> cod = cod;
    nuevaCarga -> name = name;
    nuevaCarga -> recorr = strToTime(tiempoRecorr);
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
    nuevoGrupo -> hora = strToTime(hora);
    nuevoGrupo -> cantidad = cantidad;

    anadirNodoAlFinal(nuevoGrupo,carga->grupos);
}

/*
    Dada una cantidad se intena reducir lo mas posible esta cantidad usando el primer grupo 
    de la cola de la carga
    Argumentos:
        cantidad: la direccion donde se encuentra la cantidad que se quiere intentar reducir
        carga: Carga donde se reducira la cantidad 
    Retorna:
        un time_t con la hora del grupo que se uso para reducir la cantidad solicitada (no 
        necesariamente se reduce toda la cantidad que se requiere puede reducirse menos pero
        se actualiza el contenido de cantidad)
*/
time_t reducirCarga(int *cantidad, t_carga *carga) {
    t_grupo *contenido;
    contenido = (t_grupo *)(carga->grupos->siguiente->contenido);
    time_t tiempo = contenido->hora;
        
    if (contenido == NULL)
        return;

    if((contenido->cantidad - *cantidad) <= 0) {/*la cantidad solicitada es mayor a la cantidad de persona en espera*/
        *cantidad = *cantidad - contenido->cantidad;
        eliminarNodo(carga->grupos->siguiente);
        
    } else if ((contenido->cantidad - *cantidad) > 0) {
        contenido->cantidad = contenido->cantidad - *cantidad;
        *cantidad = 0;
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
    free(carga);
}
