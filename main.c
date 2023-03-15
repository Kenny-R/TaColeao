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

#define MAX_ORIGEN_LENGTH 10
#define MAX_DESTINO_LENGTH 10
#define MAX_HORA_LENGTH 6
#define MAX_MENSAJE_LENGTH 30

/* Seccion de codigo compartida por todos*/
int nroRutasLoads;
t_carga **loads;
int nroRutasRouteServices;
itinerario **route_services;
time_t hora_actual;
int finalizar_reloj = 0;
double t;

void *updatetime(void *arg)
{
    while (!finalizar_reloj)
    {
        hora_actual = hora_actual + 60;        
        sleep(1);
    }
    pthread_exit(NULL);
}

void enviarMensaje(int *fd, char origen[], char destino[], time_t *hora, char mensaje[]) {
    
    if (strlen(origen) > MAX_ORIGEN_LENGTH ||
        strlen(destino) > MAX_DESTINO_LENGTH||
        strlen(mensaje) > MAX_MENSAJE_LENGTH) {
            printf("No se puede enviar un mensaje tan grande\n");
            return;
        }
    /*Creamos el str de la hora*/
    char time[6];
    strftime(time,6,"%H:%M",localtime(hora));
    
    /*calculamos parte del tamaño del mensaje*/
    int n = strlen(origen) + 1 + strlen(destino) + 1 + strlen(time) + 1 + strlen(mensaje)+1;
    
    /*tansfromamos el entro n en un string*/
    char largo[sizeof(int)*8+1];
    sprintf(largo,"%d",n);

    /*Calculamos el tamaño total del mensaje*/
    int m = n + strlen(largo) + 1;
    
    /*creamos el mensaje final*/
    char mensajeFinal[m];
    sprintf(mensajeFinal,"%d|%s|%s|%s|%s\0",n,origen,destino,time,mensaje);
    /*Enviamos el mensaje*/
    write(*fd,mensajeFinal,m);
}

void leerMensaje(int *fd, char origen[], char destino[], time_t *hora, char mensaje[]) {
    /*leemos la primera parte del mensaje hasta que encontremos un "|" */
    char c[2];
    char largo[sizeof(int)*8+1] = "";
    while (1) {
        read(*fd,c,1);
        c[1] = '\0';
        if (c[0] != '|') {
            strcat(largo,c);
        } else {
            break;
        }
    }

    int n = atoi(largo);
    char contenido[n];
    read(*fd,contenido,n);
    char time[6];
    sscanf(contenido,"%[^|]|%[^|]|%[^|]|%[^|]",origen,destino,time,mensaje);
    *hora = strToTime(time);
}

void controlRuta(itinerario *infoRuta, t_carga *infCarga, int *pipeLectura, int *pipeEscritura, int t)
{
    char origen[MAX_ORIGEN_LENGTH];
    char destino[MAX_DESTINO_LENGTH];
    time_t hora;
    char mensaje[MAX_MENSAJE_LENGTH];
    
    /* reviso si puedo empezar 
    while (1)
    {
        /* printf("Estoy esperando para comenzar %s \n", infoRuta->cod); 
        leerMensaje(pipeLectura,origen,destino,&hora,mensaje);
        if (strcmp(mensaje, "Empieza") == 0)
            break;
    }
    */

    nodo *nodoServicioActual = infoRuta->servicios->siguiente;
    servicio_autobus *contenido = (servicio_autobus *)(nodoServicioActual->contenido);
    /* reviso si el proceso padre mando una señal al hijo */
    
    while (1)
    {
        leerMensaje(pipeLectura, origen, destino, &hora, mensaje);
        if (strcmp(mensaje, "Actualiza\n") == 0)
        {
            /* actualizo */
            while (nodoServicioActual->contenido != NULL && difftime(contenido->hora, hora) <= 0)
            {
                nodoServicioActual = nodoServicioActual->siguiente;
                contenido = (servicio_autobus *)(nodoServicioActual->contenido);
            }
            printf("%s tiene hora %d:%d\n", infoRuta->cod, localtime(&contenido->hora)->tm_hour, localtime(&contenido->hora)->tm_min);
            hora = hora + 60;
            if (nodoServicioActual->contenido == NULL)
                break;
            else
                enviarMensaje(pipeEscritura, infoRuta->cod, "padre", &hora, "NO HE TERMINADO\n");
            }
    }

    /* printf("Empezando %s \n", infoRuta->cod);   */
    /* printf("Se creo la ruta %s\n", infoRuta->cod);
    printf("trabajando....\n"); */
    /* sleep(1); */
    /* printf("Se elimino la ruta %s\n", infoRuta->cod); */
    
    /* le aviso al proceso padre de que termine */
    enviarMensaje(pipeEscritura, infoRuta->cod, "padre", &hora, "Adios\n");
    printf("Adios %s\n", infoRuta->cod);
    close(*pipeLectura); /* cierro la parte de lectura del pipe de comunicacion */
    exit(0);
}

int main(int argc, char *argv[])
{
    char archivoCarga[MAX_NAME_FILE] = "./datos/carga.csv";
    char archivoServicio[MAX_NAME_FILE] = "./datos/servicio2019.txt";
    t = 0.25;
    const tmin = (int)(t * 1000000);

    if (comprobarEntrada(argc,argv,archivoCarga,archivoServicio,&t) != 1){
        return EXIT_FAILURE;
    }
    
    hora_actual = strToTime("6:00");
    printf("hora actual: ");
    imprimirHora(&hora_actual);

    /* conseguimos los datos*/
    loads = leerCarga(archivoCarga, &nroRutasLoads);
    route_services = leerServicio(archivoServicio, &nroRutasRouteServices);

    /* pid de los procesos (es decir, de las paradas de los autobuses) */
    pid_t ruta[nroRutasRouteServices];

    int i;
    /* pipe para comunicacion de proceso padre a cada hijo */
    int pipesPadreHijo[nroRutasRouteServices][2];
    /* pipe para comunicacion de cada hijo al proceso padre */
    int pipesHijoPadre[nroRutasRouteServices][2];
    for (i = 0; i < nroRutasRouteServices; i++)
    {
        pipe(pipesPadreHijo[i]);
        pipe(pipesHijoPadre[i]);

        enviarMensaje(&pipesHijoPadre[i][1], route_services[i]->cod, "padre", &hora_actual, "NO HE TERMINADO\n");
    }

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
        { /*HIJO*/
            close(pipesHijoPadre[i][0]);
            close(pipesPadreHijo[i][1]);
            controlRuta(route_services[i], buscarCarga(route_services[i]->cod, loads, nroRutasLoads),&pipesPadreHijo[i][0], &pipesHijoPadre[i][1],t);
            exit(0);
        }
        else
        { /*PADRE*/
            if (i == nroRutasRouteServices - 1)
            {
                char origen[6], destino[6], mensaje[30];
                time_t h;
                printf("Padre avisando a todos los procesos hijos \n");
                int j;
                
                
                /* arreglo para saber si un proceso hijo termino o no: 0 si no ha terminado y 1 si ha terminado */
                int finalizado[nroRutasRouteServices];
                memset(finalizado, 0, nroRutasRouteServices * sizeof(int));

                /* cerramos los lados en los que no podriamos escribir*/
                for (j=0; j< nroRutasRouteServices; j++) {
                    close(pipesHijoPadre[j][1]);
                    close(pipesPadreHijo[j][0]);
                }

                while (1)
                {
                    int cnt = 0;
                    
                    usleep(tmin);
                    hora_actual = hora_actual + 60;
                    printf("el padre tiene hora %d:%d\n", localtime(&hora_actual)->tm_hour, localtime(&hora_actual)->tm_min);
                    for (j = 0; j < nroRutasRouteServices; j++)
                    {
                        if (!finalizado[j])
                        {
                            leerMensaje(&pipesHijoPadre[j][0], origen, destino, &h, mensaje);
                            if (!strcmp(mensaje, "Adios\n"))
                            {
                                finalizado[j] = 1;
                                cnt++;
                            }
                            else
                                enviarMensaje(&pipesPadreHijo[j][1], "padre", route_services[j]->cod, &hora_actual, "Actualiza\n");
                        }
                        else
                            cnt++;
                    }
                    if (cnt == nroRutasRouteServices)
                        break;

                }
            }
        }
    }

    printf("Proceso padre: LET IT GOOOO! XDD \n");
    /*Los hijos no llegan a esta zona*/
    /*esperamos que terminen todos los hijos*/
    for (i = 0; i < nroRutasRouteServices; i++)
    {
        wait(&ruta[i]);
    }
}
