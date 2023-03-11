#ifndef LISTA_ENLAZADA_LIB
    #define LISTA_ENLAZADA_LIB
    typedef struct NODO
    {
        struct NODO * anterior;
        
        void * contenido;

        struct NODO * siguiente;
    } nodo;

    /*
        Crea una lista enlazada vacia
        Retorna:
            la cabeza de una lista enlazada. Esta cabeza debe tener contenido NULL
    */
    nodo * crearListaEnlazada();

    /*
        dado un contenido y una lista enlazada crea un nuevo nodo y lo inserta al 
        final de la lista
        Argumentos:
            contenido: El contenido que tendra el nuevo nodo
            listaEnlazada: La lista enlazada donde se insertara el nuevo nodo
    */
    void anadirNodoAlFinal(void * contenido, nodo * listaEnlazada);

    /*
        dado un contenido y una lista enlazada crea un nuevo nodo y lo inserta al 
        inicio de la lista
        Argumentos:
            contenido: El contenido que tendra el nuevo nodo
            listaEnlazada: La lista enlazada donde se insertara el nuevo nodo
    */
    void anadirNodoAlInicio(void * contenido, nodo * listaEnlazada);
  
    /*
        libera la memoria ocupada por una lista enlazada
        Argumento:
            listaEnlazada: La lista enlazada a liberar
    */
    void eliminarLista(nodo * listaEnlazada);

    /*
        Elimina un nodo de una lista enlazada y libera su espacio ocupado
        Argumento:
            nodo_a_eliminar: El nodo a eliminar (debe pertenecer a una lista enlazada)
    */
    void eliminarNodo(nodo *nodo_a_eliminar);
#endif