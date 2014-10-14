CC = c++
CFLAGS = -g -c 

all :dbFile.o Attribute.o Value.o Schema.o Record.o avl.o LRU.o Catalog.o structures.o			
	$(CC) -g main.cc dbFile.o  Attribute.o Value.o Schema.o Record.o avl.o LRU.o Catalog.o structures.o


dbFile.o: Attribute.o Value.o Schema.o Record.o avl.o LRU.o Catalog.o structures.o			
	$(CC) -c -g dbFile.cpp Attribute.o Value.o Schema.o Record.o avl.o LRU.o Catalog.o structures.o

clean:
	rm *.o

avl.o:avl.c
	gcc $(CFLAGS) avl.c


Attribute.o : Phase2/Attribute.cc	
	$(CC) -c Phase2/Attribute.cc  
	
Record.o : Phase2/Record.cc	
	$(CC) $(CFLAGS) Phase2/Record.cc 
	
Value.o : Phase2/Value.cc	
	$(CC) $(CFLAGS) Phase2/Value.cc
	
Schema.o : Phase2/Schema.cc	
	$(CC) $(CFLAGS) Phase2/Schema.cc

Catalog.o : Phase2/Catalog.cc	
	$(CC) $(CFLAGS) Phase2/Catalog.cc
	
LRU.o : LRU.cpp	
	$(CC) $(CFLAGS) LRU.cpp
	
structures.o:structures.cpp	
	$(CC) $(CFLAGS) structures.cpp

