#include "utilidades.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* Comprueba los datos de entrada si todo fue correcto un numero distinto mayor a 0 si algo fallo devuelve un numero menor
o igual a 0*/
int comprobarEntrada(int argc, char *argv[], char *nameC, double *t) {
    FILE * temp;
    /*comprobamos que la entrada tenga mas de 1 argumento*/
    if (argc < 3) {
        printf("Error, too few arguments\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n",argv[0]);
        return -1;
    }

    /* comprobamos que el -s es correcto */
     if (strcmp(argv[1],"-s") != 0){
            printf("Error with the arguments\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[0]);
            return -1;
    }

    if ((temp = fopen(argv[2], "r")) == NULL){
        printf("Error with the argument -s \"%s\"\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[2], argv[0]);
        return -1;
    }
    fclose(temp);

    if (argc == 5) {
        if (strcmp(argv[3],"-c") == 0) { /* si existe comprobamos que el -c es correcto*/
            
            if ((temp = fopen(argv[4], "r")) == NULL){
                printf("Error with the argument -c \"%s\"\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[4], argv[0]);
                return -1;
            }
            fclose(temp);
            memset(nameC,0,MAX_NAME);
            strcpy(nameC,argv[4]);

        } else if (strcmp(argv[3],"-t") == 0) { /* si existe comprobamos que el -t es correcto*/
            if (atof(argv[4]) <= 0){ /* no puedes tener un tiempo de 0 o negativo*/
                printf("Error with the argument -t \"%s\"\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n",argv[4],argv[0]);
                return -1;
            }

            *t = atof(argv[4]);

            if ((temp = fopen(nameC, "r")) == NULL){
                printf("Error with the file \"%s\" maybe it doesn't exists \n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", nameC, argv[0]);
                return -1;
            }
            fclose(temp);

        } else {
            printf("Error with the arguments\n Use: %s -s <file name> [-c <file name>] [-t <num>]\n", argv[0]);
            return -1;
        }
    } else if (argc == 7) {
        if ((strcmp(argv[3],"-c") != 0) || ((temp = fopen(argv[4], "r")) == NULL)){
                printf("Error with the argument -c \n Use: %s -s <file name> [-c <file name>] [-t <num>]\n",argv[0]);
                return -1;
            }
        fclose(temp);
        
        memset(nameC,0,MAX_NAME);
        strcpy(nameC,argv[4]);
        
        if ((strcmp(argv[5],"-t") != 0) || (atof(argv[6]) <= 0)){ /* no puedes tener un tiempo de 0 o negativo*/
                printf("Error with the argument -t \n Use: %s -s <file name> [-c <file name>] [-t <num>]\n",argv[0]);
                return -1;
            }
            
        *t = atof(argv[6]);
    }

    return 0;
};