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

void controlRuta(itinerario *infoRuta, t_carga *infCarga)
{

    printf("Se creo la ruta %s\n", infoRuta->cod);
    printf("esperamos que que los demas procesos se creen\n");
    sleep(5);
    printf("Se elimino la ruta %s\n", infoRuta->cod);
    exit(0);
}

int main(int argc, char *argv[])
{
    char *a = "./datos/carga.csv";
    char *b = "./datos/servicio2019.txt";

    /* conseguimos los datos*/
    loads = leerCarga(a, &nroRutasLoads);
    route_services = leerServicio(b, &nroRutasRouteServices);

    /* pid de los procesos (es decir, de las paradas de los autobuses) */
    pid_t ruta[nroRutasRouteServices];

    int i;
    /* pipe de empezar */
    int pipeBegin[nroRutasRouteServices][2];
    for (i = 0; i < nroRutasRouteServices; i++)
        pipe(pipeBegin[i]);
    /* pipe para comunicacion de proceso padre a cada hijo */
    int fds[nroRutasRouteServices][2];
    for (i = 0; i < nroRutasRouteServices; i++)
        pipe(fds[i]);

    char buffer[20][30]; /* puse 30 para el largo del msj del pipe */

    for (i = 0; i < nroRutasRouteServices; i++)
    {
        ruta[i] = fork();
        if (ruta[i] == -1)
        { /*No se pudo crear el nuevo proceso*/
            printf("Ha habido un error en la creacion de un proceso\n");
            exit(-1);
            break;
        }
        else if (ruta[i] == 0)
        {                           /*HIJO*/
            close(pipeBegin[i][1]); /* cierro la parte de escritura del pipe de empezar */
            close(fds[i][0]);       /* cierro la parte de lectura del pipe de comunicacion */

            /* reviso si puedo empezar */
            while (1)
            {
                read(pipeBegin[i][0], buffer[i], 9);
                if (strcmp(buffer[i], "Empieza\n") == 0)
                {
                    break;
                }
            }
            printf("EMPEZANDO %s \n", route_services[i]->cod);

            controlRuta(route_services[i], buscarCarga(route_services[i]->cod, loads, nroRutasLoads));
            exit(0);
        }
        else
        {                           /*PADRE*/
            close(pipeBegin[i][0]); /* cierro la parte de lectura del pipe de empezar */
            close(fds[i][1]);       /* cierro la parte de escritura del pipe de comunicacion */

            if (i == nroRutasRouteServices - 1)
            {
                int j;
                printf("AVISO\n");
                for (j = 0; j < nroRutasRouteServices; j++)
                {
                    write(pipeBegin[j][1], "Empieza\n", 9);
                }
            }

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
