all : test_ex1 test_ex2 test_ex3



projet.o : projet.c projet.h
	gcc -Wall -c projet.c 

test_ex1.o : projet.h test_ex1.c
	gcc -Wall -c test_ex1.c

test_ex2.o : projet.h test_ex2.c
	gcc -Wall -c test_ex2.c

test_ex3.o: projet.h test_ex3.c
	gcc -Wall -c test_ex3.c

test_ex1 : test_ex1.o projet.o
	gcc -Wall -o test_ex1 test_ex1.o projet.o

test_ex2 : test_ex2.o projet.o
	gcc -Wall -o test_ex2 test_ex2.o projet.o

test_ex3: test_ex3.o projet.o
	gcc -Wall -o test_ex3 test_ex3.o projet.o

clean:
	rm -f *.o test_ex1 test_ex2 test_ex3