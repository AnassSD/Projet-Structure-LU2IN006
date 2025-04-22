all : test_ex1 



projet.o : projet.c projet.h
	gcc -Wall -c projet.c 

test_ex1.o : projet.h test_ex1.c
	gcc -Wall -c test_ex1.c

test_ex1 : test_ex1.o projet.o
	gcc -Wall -o test_ex1 test_ex1.o projet.o

clean:
	rm -f *.o test_ex1