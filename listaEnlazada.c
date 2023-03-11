#include <stdlib.h>
#include "listaEnlazada.h"
#include <stdio.h>

/*
    Crea una lista enlazada vacia
    Retorna:
        la cabeza de una lista enlazada. Esta cabeza debe tener contenido NULL
*/
nodo * crearListaEnlazada(){
    nodo * head = malloc(sizeof(nodo));

    head -> anterior = head;
    head -> siguiente = head;
    head -> contenido = NULL;

    return head;
}

/*
    dado un contenido y una lista enlazada crea un nuevo nodo y lo inserta al 
    final de la lista
    Argumentos:
        contenido: El contenido que tendra el nuevo nodo
        listaEnlazada: La lista enlazada donde se insertara el nuevo nodo
*/
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


/*
    dado un contenido y una lista enlazada crea un nuevo nodo y lo inserta al 
    inicio de la lista
    Argumentos:
        contenido: El contenido que tendra el nuevo nodo
        listaEnlazada: La lista enlazada donde se insertara el nuevo nodo
*/
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

/*
    libera la memoria ocupada por una lista enlazada
    Argumento:
        listaEnlazada: La lista enlazada a liberar
*/
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

/*
    Elimina un nodo de una lista enlazada y libera su espacio ocupado
    Argumento:
        nodo_a_eliminar: El nodo a eliminar (debe pertenecer a una lista enlazada)
*/
void eliminarNodo(nodo *nodo_a_eliminar){
    nodo_a_eliminar->anterior->siguiente = nodo_a_eliminar->siguiente;
    nodo_a_eliminar->siguiente->anterior = nodo_a_eliminar->anterior;
    free(nodo_a_eliminar);
}