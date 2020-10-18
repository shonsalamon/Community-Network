FLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors
LIBS = -lm

all: cluster

cluster: main.o graph.o division.o spmat.o glist.o linearOps.o validator.o
	gcc main.o graph.o division.o spmat.o glist.o linearOps.o validator.o -o cluster $(LIBS)

clean: 
	rm -rf *.o cluster
	
main.o: main.c glist.h graph.h spmat.h division.h validator.h linearOps.h
	gcc $(FLAGS) -c main.c
	
graph.o: graph.c graph.h validator.h
	gcc $(FLAGS) -c graph.c
	
division.o: division.c division.h validator.h linearOps.h 
	gcc $(FLAGS) -c division.c
	
spmat.o: spmat.c spmat.h validator.h
	gcc $(FLAGS) -c spmat.c
	
glist.o: glist.c glist.h validator.h
	gcc $(FLAGS) -c glist.c
	
linearOps.o: linearOps.c linearOps.h 
	gcc $(FLAGS) -c linearOps.c
	
validator.o: validator.c validator.h
	gcc $(FLAGS) -c validator.c