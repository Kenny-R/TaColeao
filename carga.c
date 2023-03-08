#include "utilidades.h"
#include "carga.h"
#include <stdio.h>
#include <stdlib.h>

void agregarGrupo(char *hora, int cantidad,t_carga *carga) {
    t_grupo *nuevoGrupo = malloc(sizeof(t_grupo));
    nuevoGrupo -> hora = strToTime(hora);
    nuevoGrupo -> cantidad = cantidad;

    anadirNodoAlFinal(nuevoGrupo,carga->grupos);
}
