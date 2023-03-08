#ifndef CARGA_LIB 
    #define CARGA_LIB
    
    #include <time.h>
    #include "listaEnlazada.h"
    
    typedef struct {
        time_t hora;
        int cantidad;
    } t_grupo;

    typedef struct {
        char *cod;
        char *name;
        time_t recorr;
        nodo *grupos;
    } t_carga;

    void agregarGrupo(char *hora, int cantidad,t_carga *carga);
#endif