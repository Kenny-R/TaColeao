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

#define READ_END 0
#define WRITE_END 1

/* Seccion de codigo compartida por todos*/
int nroRutasLoads, nroRutasRouteServices;
t_carga **loads;
itinerario **route_services;
time_t hora_actual;
int finalizar_reloj = 0;
int nroPuntuales = 0;
double t = 0.25;

/*
    Funcion del hilo (autobus)
    Actualiza el estado de porcentaje de ida (si el autobus va de uni a parada) y de regreso (si va de parada a uni)
    Argumentos:
        dataAvance:  el apuntador de una funcion que luego vamos a convertirlo en un struct "avance"
                    donde contiene los datos necesarios para que el hilo pueda actualizar por sí mismo su estado
*/
void *autobus(void *dataAvance)
{
    struct avance *miAvance = (struct avance *)dataAvance;
    int i = miAvance->pos;
    int *porcentaje = miAvance->arrPorcentajes;
    time_t recorr = miAvance->tiempoRecorr;
    time_t horaPartida = miAvance->horaPartida;
    time_t *hora = miAvance->horaActual;
    pthread_mutex_t mutexReducCarga = PTHREAD_MUTEX_INITIALIZER;
    int espera = 10;

    /* calculemos cuantos minutos han pasado desde que el autobus partio hacia la parada */
    int inicio = difftime(*hora, horaPartida) / 60;
    /* y calculemos cuantos minutos tarda en el recorrido */
    int minRecorr = ((localtime(&recorr)->tm_hour) * 60 + localtime(&recorr)->tm_min);

    while (1)
    {
        /* el hilo espera a que el proceso hijo le indique que actualice */
        if (miAvance->go[i] == 1)
        {
            /* seccion critica */
            if (miAvance->ida != -1)
            {
                porcentaje[i] = (inicio * 100) / minRecorr;
                inicio++;
                if (porcentaje[i] >= 100)
                {
                    inicio = 0;
                    if (miAvance->ida == 1)
                    {
                        /* esperar 10t */
                        miAvance->ida = -1;
                    }
                    else if (miAvance->ida == 0)
                    {
                        /* el autobus regreso a la uni */
                        miAvance->ida = 2;
                        break;
                    }
                }
            }
            else
            {
                pthread_mutex_lock(&mutexReducCarga);
                reducirCarga(miAvance->asientos, &nroPuntuales, espera, minRecorr, miAvance->cargaParada, *hora);
                pthread_mutex_unlock(&mutexReducCarga);

                if (inicio == 10)
                {
                    inicio = 1;
                    porcentaje[i] = (inicio * 100) / minRecorr;
                    miAvance->ida = 0;
                }
                inicio++; espera--;
            }
            /* libero el semaforo */
            miAvance->go[i]--;
        }
    }

    *(miAvance->servicios_terminados) = *(miAvance->servicios_terminados) + 1;
    miAvance->go[i] = -1;
    pthread_exit(NULL);
}

/*
    Funcion de los procesos hijos (rutas)
    Espera a que el proceso padre le mande una señal para indicarle a que actualicen los hilos, que se van a crear
    segun la hora actual de la simulacion
    Argumentos:
        infoRuta: un apuntador de un struct itinerario que sirve para poder acceder a la lista de servicios de
                  una determinada ruta
        infCarga: un apuntador de un struct t_carga que contiene los datos de las cargas de una determinada ruta
        pipeLectura: un apuntador del pipe por donde va a leer el mensaje que le envía el proceso padre
        pipeEscritura: un apuntador del pipe por donde se va a escribir el mensaje al proceso padre
*/
void controlRuta(itinerario *infoRuta, t_carga *infCarga, int *pipeLectura, int *pipeEscritura, int t)
{
    char origen[MAX_ORIGEN_LENGTH];
    char destino[MAX_DESTINO_LENGTH];
    time_t hora;
    char mensaje[MAX_MENSAJE_LENGTH];

    int numero_servicios = infoRuta->numero_servicios;
    int servicios_arrancados = 0;
    int terminados = 0;

    /* arreglo de los id de los hilos */
    pthread_t idhilos[numero_servicios];

    /* un arreglo tipo struct avance, para pasarle de argumento a la funcion de los hilos */
    struct avance *avances = (struct avance *)malloc(numero_servicios * sizeof(struct avance));
    if (avances == NULL)
    {
        printf("No hay suficiente memoria disponible\n");
        exit(1);
    }

    /* arreglo para saber el porcentaje en que van los autobuses */
    int porcentajes[numero_servicios];
    memset(porcentajes, 0, numero_servicios * sizeof(int));

    /* un arreglo que indicara si el autobus tiene que actualizarse o no */
    int go[numero_servicios];
    int posSemaforo;
    for (posSemaforo = 0; posSemaforo<numero_servicios;posSemaforo++){
        go[posSemaforo] = 0;
    }

    nodo *nodoServicioActual = infoRuta->servicios->siguiente;
    servicio_autobus *contenido = (servicio_autobus *)(nodoServicioActual->contenido);
    /* reviso si el proceso padre mando una señal al hijo */
    int k, tmp = 0;
    while (TRUE)

    {
        leerMensaje(pipeLectura, origen, destino, &hora, mensaje);

        if (strcmp(mensaje, "Actualiza\n") == 0)
        {
            /* actualizo */
            for (k = terminados; k < numero_servicios; k++)
                if(go[k] == 0)
                    go[k] = 1;
            
            if (nodoServicioActual->contenido != NULL)
            {
                while (nodoServicioActual->contenido != NULL && difftime(contenido->hora, hora) <= 0)
                {
                    /* creacion de hilo */
                    avances[servicios_arrancados].asientos = &contenido->capacidad;
                    avances[servicios_arrancados].cargaParada = infCarga;
                    avances[servicios_arrancados].ida = 1;
                    avances[servicios_arrancados].pos = servicios_arrancados;
                    avances[servicios_arrancados].arrPorcentajes = porcentajes;
                    avances[servicios_arrancados].horaPartida = contenido->hora;
                    avances[servicios_arrancados].horaActual = &hora;
                    avances[servicios_arrancados].tiempoRecorr = infCarga->recorr;
                    avances[servicios_arrancados].go = go;
                    avances[servicios_arrancados].servicios_terminados = &terminados;
                    pthread_create(&idhilos[servicios_arrancados], NULL, &autobus, (void *)(avances + servicios_arrancados));

                    /* itero al siguiente nodo de la lista enlazada de servicios */
                    nodoServicioActual = nodoServicioActual->siguiente;
                    contenido = (servicio_autobus *)(nodoServicioActual->contenido);
                    servicios_arrancados++;
                }
            }
            if (servicios_arrancados == 0)
                enviarMensaje(pipeEscritura, infoRuta->cod, "padre", &hora, "No he terminado\n");
            else {
                /* Antes de enviar la informacion del estado tenemos que  esperar que  todos los autobuses se actualicen*/
                int i = 0;
                while (i != servicios_arrancados)
                    if (go[i] == 0 || go[i] == -1)
                        i++;
                
                /* Enviamos la informacion del estado*/
                strcpy(mensaje, codficarInformacion(avances, infCarga->cod, numeroDePersonasEnEspera(infCarga, hora), servicios_arrancados, MAX_MENSAJE_LENGTH));
                /* printf("envie el mensaje: %s",mensaje); */
                
                if (terminados == numero_servicios)
                    mensaje[0] = 'F';
                enviarMensaje(pipeEscritura,
                              infoRuta->cod,
                              "padre",
                              &hora,
                              mensaje);
                
                /* verifiquemos si hay un autobus que terminó su viaje, hacemos un join de los hilos que ya han terminado */
                while (tmp != terminados)
                {
                    pthread_join(idhilos[tmp], NULL);
                    tmp++;
                }
            
                /* si ya todos los autobuses terminaron su viaje y ya regresaron a la uni, hacemos un break */
                if (terminados == numero_servicios)
                    break;
            }
        }
    }
    /* le aviso al proceso padre de que termine */
    char buffer[sizeof(int) * 8 + 1];
    memset(mensaje,0,MAX_MENSAJE_LENGTH);
    strcat(mensaje,"A,");
    sprintf(buffer, "%d", nroPuntuales);
    strcat(mensaje,buffer);
    strcat(mensaje,",");
    sprintf(buffer,"%d",infCarga->pasajeros -nroPuntuales);
    strcat(mensaje,buffer);
    strcat(mensaje,"\n");

    /*pasamos todo a un string mas grande para luego pasar el mensaje*/
    /* printf("Número de personas que llegaron temprano en %s: %d\n", infoRuta->cod, nroPuntuales);
    printf("Número de personas que llegaron tarde en %s: %d\n", infoRuta->cod, infCarga->pasajeros -nroPuntuales); */
    enviarMensaje(pipeEscritura, infoRuta->cod, "padre", &hora, mensaje);

    /* cierro la parte de lectura del pipe de comunicacion */
    close(*pipeLectura); 
    /* libero la memoria malloc */
    free(avances);
    exit(0);
}


int main(int argc, char *argv[])
{
    char archivoCarga[MAX_NAME_FILE] = "./datos/carga.csv";
    char archivoServicio[MAX_NAME_FILE] = "./datos/servicio2019.txt";

    if (comprobarEntrada(argc, argv, archivoCarga, archivoServicio, &t) != 1)
        return EXIT_FAILURE;

    const tmin = (int)(t * 1000000);

    if (comprobarEntrada(argc, argv, archivoCarga, archivoServicio, &t) != 1)
        return EXIT_FAILURE;
    
    char origen[MAX_ORIGEN_LENGTH], destino[MAX_DESTINO_LENGTH], mensaje[MAX_MENSAJE_LENGTH];
    time_t h;

    hora_actual = strToTime("4:00");
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

        enviarMensaje(&pipesHijoPadre[i][WRITE_END], route_services[i]->cod, "padre", &hora_actual, "NO HE TERMINADO\n");
    }

    for (i = 0; i < nroRutasRouteServices; i++)
    {
        ruta[i] = fork();
        if (ruta[i] == -1)
        { /*No se pudo crear el nuevo proceso*/
            printf("Ha habido un error en la creacion de un proceso\n");
            exit(1);
            break;
        }
        else if (ruta[i] == 0)
        { /*HIJO*/
            close(pipesHijoPadre[i][READ_END]);
            close(pipesPadreHijo[i][WRITE_END]);
            controlRuta(route_services[i], buscarCarga(route_services[i]->cod, loads, nroRutasLoads), &pipesPadreHijo[i][READ_END], &pipesHijoPadre[i][WRITE_END], t);
            exit(0);
        }
        else
        { /*PADRE*/
            if (i == nroRutasRouteServices - 1)
            {
                int j;

                printf("Iniciamos la simulacion \n");
                /* arreglo para saber si un proceso hijo termino o no: 0 si no ha terminado y 1 si ha terminado */
                int finalizado[nroRutasRouteServices];
                memset(finalizado, 0, nroRutasRouteServices * sizeof(int));

                /* cerramos los lados en los que no podriamos escribir*/

                for (j = 0; j < nroRutasRouteServices; j++)
                {
                    close(pipesHijoPadre[j][WRITE_END]);
                    close(pipesPadreHijo[j][READ_END]);
                }

                while (TRUE)
                {
                    int cnt = 0;
                    usleep(tmin);
                    hora_actual = hora_actual + 60;
                    /* printf("el padre tiene hora %d:%d\n", localtime(&hora_actual)->tm_hour, localtime(&hora_actual)->tm_min); */
                    printf("Hora Actual: ");
                    imprimirHora(&hora_actual);
                    for (j = 0; j < nroRutasRouteServices; j++)
                    {
                        if (!finalizado[j])
                        {
                            leerMensaje(&pipesHijoPadre[j][READ_END], origen, destino, &h, mensaje);
                            if (mensaje[0] == 'F')
                            {   
                                imprimirMsg(route_services[j]->cod, mensaje);
                                printf("parada de %s ha finalizado\n", route_services[j]->cod);
                                fflush(stdout);
                                finalizado[j] = 1;
                                cnt++;
                            }

                            if (finalizado[j] != 1)
                                enviarMensaje(&pipesPadreHijo[j][WRITE_END], "padre", route_services[j]->cod, &hora_actual, "Actualiza\n");
                            
                            if (mensaje[0] == 'I')
                            {
                                imprimirMsg(route_services[j]->cod, mensaje);
                            }
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

    /*Los hijos no llegan a esta zona*/
    /*esperamos que terminen todos los hijos*/
    for (i = 0; i < nroRutasRouteServices; wait(&ruta[i++]));
    int aTiempo;
    int tarde;
    char basura;
    for (i = 0; i<nroRutasRouteServices; i++) {
        leerMensaje(&pipesHijoPadre[i][READ_END], origen, destino, &h, mensaje);

        if (mensaje[0] == 'A'){
            sscanf(mensaje,"%c,%d,%d",&basura,&aTiempo,&tarde);
            printf("Parada %s\nLlegaron a tiempo: %d\nLlegaron tarde: %d\n",route_services[i]->cod,aTiempo,tarde);
        }
    }
    
    int j;
    for (i = 0; i < nroRutasLoads; i++)
    {
        int encontrado = 0;
        for (j = 0; j < nroRutasRouteServices; j++)
        {
            if (strcmp(loads[i]->cod, route_services[j]->cod) == 0)
            {
                encontrado = 1;
                break;
            }
        }
        if (!encontrado)
        {
            printf("Parada %s\nLlegaron a tiempo: %d\nLlegaron tarde: %d\n", loads[i]->cod, 0, loads[i]->pasajeros);
        }
    }
    
    printf("Termino la simulacion.\n");
    
    for (i = 0; i<nroRutasRouteServices; i++) {
        eliminarItinerario(route_services[i]);
    }

    for (i=0; i<nroRutasLoads;i++) {
        eliminarCargar(loads[i]);
    }

    return EXIT_SUCCESS;
}
