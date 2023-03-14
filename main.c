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
struct tm *hora_actual;
int finalizar_reloj = 0;

void *updatetime(void *arg)
{
    while (!finalizar_reloj)
    {
        printf("%d:%d:%d\n", hora_actual->tm_hour, hora_actual->tm_min, hora_actual->tm_sec);
        if (hora_actual->tm_sec == 59)
        {
            hora_actual->tm_min = hora_actual->tm_min + 1;
            hora_actual->tm_sec = 0;
        }
        if (hora_actual->tm_min == 59)
        {
            hora_actual->tm_hour = hora_actual->tm_hour + 1;
            hora_actual->tm_min = 0;
            hora_actual->tm_sec = 0;
        }
        hora_actual->tm_sec++;
        sleep(1);
    }
    pthread_exit(NULL);
}

void controlRuta(itinerario *infoRuta, t_carga *infCarga)
{

    printf("Se creo la ruta %s\n", infoRuta->cod);
    printf("esperamos que que los demas procesos se creen\n");
    printf("Se elimino la ruta %s\n", infoRuta->cod);
    exit(0);
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
    /* pipe para comunicacion de proceso padre a cada hijo */
    int fds[nroRutasRouteServices][2];
    for (i = 0; i < nroRutasRouteServices; i++)
        pipe(fds[i]);

    char buffer[nroRutasRouteServices][30]; /* puse 30 para el largo del msj del pipe */

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
            /* reviso si puedo empezar */
            while (1)
            {
                printf("Estoy esperando para comenzar %s \n", route_services[i]->cod);
                /*read(fds[i][0], buffer[i], 9);*/
                char origen[MAX_ORIGEN_LENGTH];
                char destino[MAX_DESTINO_LENGTH];
                time_t hora;
                char mensaje[MAX_MENSAJE_LENGTH];
                leerMensaje(&fds[i][0],origen,destino,&hora,mensaje);
                if (strcmp(mensaje, "empieza") == 0)
                    break;
            }
            printf("Empezando %s \n", route_services[i]->cod);

            close(fds[i][0]); /* cierro la parte de lectura del pipe de comunicacion */

            controlRuta(route_services[i], buscarCarga(route_services[i]->cod, loads, nroRutasLoads));
            exit(0);
        }
        else
        { /*PADRE*/
            if (i == nroRutasRouteServices - 1)
            {
                printf("Padre avisando a todos los procesos hijos \n");
                int j;
                for (j = 0; j < nroRutasRouteServices; j++)
                {
                    /*write(fds[j][1], "Empieza\n", 9);*/
                    time_t horaDeEnvio = time(NULL);
                    enviarMensaje(&fds[j][1],"padre",route_services[i]->cod,&horaDeEnvio,"empieza");
                    close(fds[j][1]); /* cierro la parte de escritura del pipe */
                }
                
                /* inicializo el reloj a las 6:00 */
                time_t s;
                hora_actual = localtime(&s);
                hora_actual->tm_hour = 6;
                hora_actual->tm_min = 0;
                
                /* creo un hilo para que actualice el reloj */
                pthread_t ptid;
                pthread_create(&ptid, NULL, &updatetime, NULL);
                
                /* por si quieren ver como el hilo actualiza el reloj xDD */
                while (1)
                {
                    printf("%d:%d:%d\n", hora_actual->tm_hour, hora_actual->tm_min, hora_actual->tm_sec);
                    sleep(1);
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
