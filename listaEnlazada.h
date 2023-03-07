#ifndef LISTA_ENLAZADA_LIB
    #define LISTA_ENLAZADA_LIB
    typedef struct NODO
    {
        struct NODO * anterior;
        
        void * contenido;

        struct NODO * siguente;
    } nodo;

    nodo * crearListaEnlazada();

    void anadirNodo(void * contenido, nodo * ListaEnlazada);

    void eliminarLista(nodo * listaEnlazada);
#endif