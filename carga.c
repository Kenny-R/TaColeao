#include "utilidades.h"
#include "carga.h"
#include <stdio.h>
#include <stdlib.h>

t_carga *crearCarga(char *cod, char *name, char *tiempoRecorr){
    t_carga *nuevaCarga = malloc(sizeof(t_carga));
    nuevaCarga -> cod = cod;
    nuevaCarga -> name = name;
    nuevaCarga -> recorr = strToTime(tiempoRecorr);
    nuevaCarga -> grupos = crearListaEnlazada();

    return nuevaCarga;
}

t_grupo *crearGrupo(char *hora, int cantidad) {
    t_grupo *nuevoGrupo = malloc(sizeof(t_grupo));
    nuevoGrupo->cantidad = cantidad;
    nuevoGrupo -> hora = strToTime(hora);

    return nuevoGrupo;
}
void agregarGrupo(char *hora, int cantidad,t_carga *carga) {
    t_grupo *nuevoGrupo = malloc(sizeof(t_grupo));
    nuevoGrupo -> hora = strToTime(hora);
    nuevoGrupo -> cantidad = cantidad;

    anadirNodoAlFinal(nuevoGrupo,carga->grupos);
}

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


void eliminarCargar(t_carga *carga) {
    eliminarLista(carga->grupos);
    free(carga);
}
