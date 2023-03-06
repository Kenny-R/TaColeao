COMP = gcc -c
LINK = gcc
OBJDIR = etc
OBJECTS = $(OBJDIR)/main.o $(OBJDIR)/utilidades.o 

PROGRAM = TaColeao.out

.PHONY: all clean

all: $(PROGRAM)

clean: 
	rm $(PROGRAM)
	rm $(OBJECTS)
	rm -r ./$(OBJDIR)

$(PROGRAM): $(OBJECTS)
	$(LINK) $(OBJECTS) -o $(PROGRAM) -lm -ansi

$(OBJDIR)/main.o: main.c $(OBJDIR) utilidades.h
	$(COMP) main.c -o $(OBJDIR)/main.o -ansi

$(OBJDIR)/utilidades.o: utilidades.c $(OBJDIR) utilidades.h
	$(COMP) utilidades.c -o $(OBJDIR)/utilidades.o -ansi

$(OBJDIR):
	mkdir $(OBJDIR)
