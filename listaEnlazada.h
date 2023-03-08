#ifndef LISTA_ENLAZADA_LIB
    #define LISTA_ENLAZADA_LIB
    typedef struct NODO
    {
        struct NODO * anterior;
        
        void * contenido;

        struct NODO * siguiente;
    } nodo;

    nodo * crearListaEnlazada();

    void anadirNodoAlFinal(void * contenido, nodo * listaEnlazada);

    void anadirNodoAlInicio(void * contenido, nodo * listaEnlazada);

    void eliminarLista(nodo * listaEnlazada);

    void eliminarNodo(nodo *nodo_a_eliminar);
#endif