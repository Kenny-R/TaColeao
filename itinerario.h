#ifndef ITINERARIO_LIB
    #define ITINERARIO_LIB
    #include "listaEnlazada.h"
    #include <time.h>
    typedef struct {
        char *cod;
        nodo *servicios;
    } itinerario;

    typedef struct {
        int capacidad;
        time_t hora;
    } servicio_autobus;

    itinerario *crearItinerario(char *cod);

    void eliminarItinerario(itinerario *itinerario_a_eliminar);

    servicio_autobus *crearServicio(char *hora, int capacidad);

    void agregarServicio(char *hora, int capacidad, itinerario *itinerario_parada);

    servicio_autobus *tomarServicio(itinerario *itinerario_parada);

#endif