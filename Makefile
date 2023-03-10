COMP = gcc -c
LINK = gcc
OBJDIR = etc
OBJECTS = $(OBJDIR)/main.o $(OBJDIR)/utilidades.o $(OBJDIR)/listaEnlazada.o $(OBJDIR)/carga.o $(OBJDIR)/itinerario.o 

PROGRAM = TaColeao.out

.PHONY: all clean

all: $(PROGRAM)

clean: 
	rm $(PROGRAM)
	rm $(OBJECTS)
	rm -r ./$(OBJDIR)

$(PROGRAM): $(OBJECTS)
	$(LINK) $(OBJECTS) -o $(PROGRAM) -lpthread -ansi

$(OBJDIR)/main.o: main.c $(OBJDIR) utilidades.h
	$(COMP) main.c -o $(OBJDIR)/main.o -ansi

$(OBJDIR)/utilidades.o: utilidades.c $(OBJDIR) carga.h $(OBJDIR) itinerario.h $(OBJDIR) utilidades.h
	$(COMP) utilidades.c -o $(OBJDIR)/utilidades.o -ansi

$(OBJDIR)/listaEnlazada.o: listaEnlazada.c $(OBJDIR) listaEnlazada.h
	$(COMP) listaEnlazada.c -o $(OBJDIR)/listaEnlazada.o -ansi

$(OBJDIR)/carga.o: carga.c $(OBJDIR) listaEnlazada.h $(OBJDIR) carga.h $(OBJDIR) utilidades.h
	$(COMP) carga.c -o $(OBJDIR)/carga.o -ansi

$(OBJDIR)/itinerario.o: itinerario.c $(OBJDIR) listaEnlazada.h $(OBJDIR) itinerario.h $(OBJDIR) utilidades.h
	$(COMP) itinerario.c -o $(OBJDIR)/itinerario.o -ansi

$(OBJDIR):
	mkdir $(OBJDIR)
