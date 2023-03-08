#include <stdlib.h>
#include "listaEnlazada.h"
#include <stdio.h>


nodo * crearListaEnlazada(){
    nodo * head = malloc(sizeof(nodo));

    head -> anterior = head;
    head -> siguiente = head;
    head -> contenido = NULL;

    return head;
}

void anadirNodoAlFinal(void * contenido, nodo * listaEnlazada) {

    nodo * nuevoNodo = malloc(sizeof(nodo));
    /*seteamos el nuevo nodo*/
    nuevoNodo -> siguiente = listaEnlazada;
    nuevoNodo -> anterior = listaEnlazada -> anterior;
    nuevoNodo -> contenido = contenido;

    /*actualizamos los apuntadores*/
    listaEnlazada -> anterior -> siguiente = nuevoNodo;
    listaEnlazada -> anterior = nuevoNodo;
}

void anadirNodoAlInicio(void * contenido, nodo * listaEnlazada) {

    nodo * nuevoNodo = malloc(sizeof(nodo));
    /*seteamos el nuevo nodo*/
    nuevoNodo -> siguiente = listaEnlazada->siguiente;
    nuevoNodo -> anterior = listaEnlazada;
    nuevoNodo -> contenido = contenido;

    /*actualizamos los apuntadores*/
    listaEnlazada -> siguiente = nuevoNodo;
    nuevoNodo-> siguiente -> anterior = nuevoNodo;
}

void eliminarLista(nodo * listaEnlazada) {

    /*vamos eliminando del ultimo hasta el primero*/
    nodo *actual = listaEnlazada ->anterior;
    while (actual->contenido != NULL) {
        actual = actual -> anterior;
        free(actual->siguiente);
    } 

    /*si llegamos aqui es que estamos en la cabeza de la lista*/
    free(listaEnlazada);
    
}