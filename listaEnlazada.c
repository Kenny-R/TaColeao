#include <stdlib.h>
#include "listaEnlazada.h"
#include <stdio.h>


nodo * crearListaEnlazada(){
    nodo * head = malloc(sizeof(nodo));

    head -> anterior = head;
    head -> siguente = head;
    head -> contenido = NULL;

    return head;
}

void anadirNodo(void * contenido, nodo * listaEnlazada) {

    nodo * nuevoNodo = malloc(sizeof(nodo));
    /*seteamos el nuevo nodo*/
    nuevoNodo -> siguente = listaEnlazada;
    nuevoNodo -> anterior = listaEnlazada -> anterior;
    nuevoNodo -> contenido = contenido;

    /*actualizamos los apuntadores*/
    listaEnlazada -> anterior -> siguente = nuevoNodo;
    listaEnlazada -> anterior = nuevoNodo;
}

void eliminarLista(nodo * listaEnlazada) {

    /*vamos eliminando del ultimo hasta el primero*/
    nodo *actual = listaEnlazada ->anterior;
    while (actual->contenido != NULL) {
        actual = actual -> anterior;
        free(actual->siguente);
    } 

    /*si llegamos aqui es que estamos en la cabeza de la lista*/
    free(listaEnlazada);
    
}