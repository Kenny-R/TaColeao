#include "itinerario.h"
#include <stdlib.h>
#include "utilidades.h"

itinerario *crearItinerario(char *cod){
    itinerario *nuevoItinerario = malloc(sizeof(itinerario));
    nuevoItinerario ->cod = cod;
    nuevoItinerario ->servicios = crearListaEnlazada();

    return nuevoItinerario;
}

void eliminarItinerario(itinerario *itinerario_a_eliminar) {
    eliminarLista(itinerario_a_eliminar->servicios);
    free(itinerario_a_eliminar);
}

servicio_autobus *crearServicio(char *hora, int capacidad) {
    servicio_autobus *nuevoServicio = malloc(sizeof(servicio_autobus));
    nuevoServicio -> capacidad = capacidad;
    nuevoServicio -> hora =  strToTime(hora);

    return nuevoServicio;
}

void agregarServicio(char *hora, int capacidad, itinerario *itinerario_parada) {
    servicio_autobus *nuevoServicio = crearServicio(hora,capacidad);
    anadirNodoAlFinal(nuevoServicio, itinerario_parada->servicios);
}

servicio_autobus *tomarServicio(itinerario *itinerario_parada) {
    servicio_autobus *servicio_a_tomar = (servicio_autobus *)(itinerario_parada->servicios->siguiente->contenido);
    if (servicio_a_tomar) {
        itinerario_parada->servicios->siguiente->siguiente->anterior = itinerario_parada->servicios;
        itinerario_parada->servicios->siguiente = itinerario_parada->servicios->siguiente->siguiente;
    }
    
    return servicio_a_tomar;
}