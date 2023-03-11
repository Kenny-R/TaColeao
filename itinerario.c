#include "itinerario.h"
#include <stdlib.h>
#include "utilidades.h"

/*
    Dado un codigo crea un itinerario para esa parada
    Argumento:
        cod: la direccion de memoria donde se encuentra el codigo de la 
             parada a la cual se le creara el itinerario
    Retorna:
        un apuntador al itinerario de esa parada
*/
itinerario *crearItinerario(char *cod){
    itinerario *nuevoItinerario = malloc(sizeof(itinerario));
    nuevoItinerario ->cod = cod;
    nuevoItinerario ->servicios = crearListaEnlazada();

    return nuevoItinerario;
}

/*
    libera la memoria ocupada por el itinerario
    Argumento:
        itinerario_a_eliminar: el itinerario que se va a liberar
*/
void eliminarItinerario(itinerario *itinerario_a_eliminar) {
    eliminarLista(itinerario_a_eliminar->servicios);
    free(itinerario_a_eliminar);
}

/*
    dado una hora y una capacidad crea un servicio de autobus
    Argumentos:
        hora: la direccion de memoria donde se encuentra la hora del servicio de autobus
        capacidad: la capacidad del autobus
    retorna:
        una struct servicio_autobus que contiene toda esta informacion
*/
servicio_autobus *crearServicio(char *hora, int capacidad) {
    servicio_autobus *nuevoServicio = malloc(sizeof(servicio_autobus));
    nuevoServicio -> capacidad = capacidad;
    nuevoServicio -> hora =  strToTime(hora);

    return nuevoServicio;
}


/*
    Dado una hora, una capacidad y un itinerario crea un servicio de autobus y lo agrega al final del itinerario
    Argumentos:
        hora: La hora a la que llega el autobus
        capacidad: la capacidad del autobus
        itinerario: el itinerario donde se agregara el servicio
*/
void agregarServicio(char *hora, int capacidad, itinerario *itinerario_parada) {
    servicio_autobus *nuevoServicio = crearServicio(hora,capacidad);
    anadirNodoAlFinal(nuevoServicio, itinerario_parada->servicios);
}

/*
    Si hay un servicio en el itinerario de parada lo saca del itinerario y lo retorna
    Argumentos: 
        itinerario_parada: El itinerario de donde se sacar un servicio.
    Retorna:
        el primer servicio de autobus que se pueda tomar del itinerario
*/
servicio_autobus *tomarServicio(itinerario *itinerario_parada) {
    servicio_autobus *servicio_a_tomar = (servicio_autobus *)(itinerario_parada->servicios->siguiente->contenido);
    if (servicio_a_tomar) {
        itinerario_parada->servicios->siguiente->siguiente->anterior = itinerario_parada->servicios;
        itinerario_parada->servicios->siguiente = itinerario_parada->servicios->siguiente->siguiente;
    }
    
    return servicio_a_tomar;
}