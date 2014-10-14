CC = g++ -Wno-deprecated -g

all: Attribute.o Value.o Record.o Schema.o Catalog.o main.o 
	$(CC) -w -o a.out Attribute.o Value.o Record.o Schema.o Catalog.o main.o

clean:
	rm -f *.out
	rm -f *.o

Attribute.o : Attribute.cc
	$(CC) -w -c Attribute.cc

Value.o : Value.cc
	$(CC) -w -c Value.cc

Record.o : Record.cc
	$(CC) -w -c Record.cc

Schema.o : Schema.cc
	$(CC) -w -c Schema.cc

Catalog.o : Catalog.cc
	$(CC) -w -c Catalog.cc

main.o : main.cc
	$(CC) -w -c main.cc
