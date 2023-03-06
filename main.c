#include "utilidades.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Funcion temporal del hilo */ 
void func(void *ptr)
{
    while (1)
    {
        int idh = *(int *)ptr;
        printf("Inside the thread #%d \n", idh);
        sleep(10);
    }
    return;
}


int main(int argc, char *argv[])
{
    char * carga = malloc(MAX_NAME);
    strcpy(carga,"carga.csv");
    double t = 0.25;
    comprobarEntrada(argc,argv,carga,&t);
    
    printf("nameC= %s\nt=%f\n",carga,t);
    
    free(carga);
    
    
    /* Creacion de los procesos e hilos */

    int n = 20; /* numero de procesos */
    int m = 5;  /* numero de hilos */

    /* PIPE */
    int fds[n][2];
    char buffer[20][30]; /* puse 30 para el largo del msj del pipe */

    for (int i = 0; i < n; i++)
        pipe(fds[i]);

    /* pid de los procesos (es decir, de las paradas de los autobuses) */
    pid_t terminal[20];

    /* hilos */
    pthread_t hilos[n][m];

    /* creacion de los n procesos */
    for (int i = 0; i < n; i++)
    {
        terminal[i] = fork();
        if (terminal[i] == -1)
        {
            printf("Ha habido un error en la creacion del proceso\n");
            exit(-1);
            break;
        }
        else if (terminal[i] == 0)
        {
            /* SOY EL HIJO */
            terminal[i] = getpid();

            close(fds[i][0]); /* cierro la parte de lectura del pipe */
            int id[m];        /* ID de los hilos (autobuses) */
            
            /* creo los hilos */
            for (int j = 0; j < m; j++)
            {
                id[j] = i * 10 + j;
                pthread_create(&hilos[i][j], NULL, (void *)&func, (void *)&id[j]);
            }
                
            /* aun no finalizo el proceso hijo */
            while (1)
            {
                printf("El proceso %d escribe\n", terminal[i]);
                write(fds[i][1], "LISTO\n", 7);
                sleep(5);
            }
            break;
        }
        else
        {
            /* SOY EL PADRE*/
            close(fds[i][1]); /* cierro la parte de escritura del pipe*/
            if (i != n - 1)
                continue;
            else
            {
                while (1)
                {
                    for (int i = 0; i < n; i++)
                    {
                        read(fds[i][0], buffer[i], 7); /* reviso lo que escribieron cada proceso */
                        printf("El padre lee: %s \n", buffer[i]);
                    }
                    sleep(10);
                }
            }
        }
    }

    /* Finalizo los procesos hijos */
    for (int i = 0; i < n; i++)
    {
        wait(NULL);
    }
    
    
    
    return 0;

}
