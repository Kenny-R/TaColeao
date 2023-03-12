#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include "listaEnlazada.h"
#include "utilidades.h"
#include "carga.h"
#include "itinerario.h"

/* Seccion de codigo compartida por todos*/
int nroRutasLoads;
t_carga **loads;
int nroRutasRouteServices;
itinerario **route_services;

void controlRuta(itinerario *infoRuta, t_carga *infCarga) {
    
    printf("Se creo la ruta %s\n",infoRuta->cod);
    printf("esperamos que que los demas procesos se creen\n");
    sleep(5);
    printf("Se elimino la ruta %s\n",infoRuta->cod);
    exit(0);
}

int main(int argc, char *argv[])
{
    char *a = "./datos/carga.csv";
    char *b = "./datos/servicio2019.txt";
    
    /* conseguimos los datos*/
    loads = leerCarga(a, &nroRutasLoads);
    route_services = leerServicio(b,&nroRutasRouteServices);

    /* Creamos los porcesos*/
    /*inicializamos el semaforo de inico para que los procesos arraquen al mismo tiempo*/
    sem_init(&inicio,1,0);

    /* pid de los procesos (es decir, de las paradas de los autobuses) */
    pid_t ruta[nroRutasRouteServices];

    int i;
    for (i= 0; i< nroRutasRouteServices; i++){
        ruta[i] = fork();
        if (ruta[i] == -1) { /*No se pudo crear el nuevo proceso*/
            printf("Ha habido un error en la creacion de un proceso\n");
            exit(-1);
            break;
        } else if (ruta[i] == 0){ /*HIJO*/
            controlRuta(route_services[i],buscarCarga(route_services[i]->cod,loads,nroRutasLoads));
            exit(0);
            
        } else { /*PADRE*/
            continue;
        } 
            
    }

    /*Los hijos no llegan a esta zona*/
    /*esperamos que terminen todos los hijos*/
    for (i = 0; i < nroRutasRouteServices; i++)
    {
        wait(&ruta[i]);
    }


}

