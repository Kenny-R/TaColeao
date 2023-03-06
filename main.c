#include "utilidades.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
    char * carga = malloc(MAX_NAME);
    strcpy(carga,"carga.csv");
    double t = 0.25;
    comprobarEntrada(argc,argv,carga,&t);
    
    printf("nameC= %s\nt=%f\n",carga,t);
    
    free(carga);
    return 0;

}
