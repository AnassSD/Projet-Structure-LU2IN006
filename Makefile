all : test_ex1 test_ex2 test_ex3 test_ex4 test_ex5 test_ex6 main

projet.o : projet.c projet.h
	gcc -Wall -g -c projet.c 

test_ex1.o : projet.h test_ex1.c
	gcc -Wall -g -c test_ex1.c

test_ex2.o : projet.h test_ex2.c
	gcc -Wall -g -c test_ex2.c

test_ex3.o: projet.h test_ex3.c
	gcc -Wall -g -c test_ex3.c

test_ex4.o: projet.h test_ex4.c
	gcc -Wall -g -c test_ex4.c


test_ex5.o: projet.h test_ex5.c
	gcc -Wall -g -c test_ex5.c

test_ex6.o: projet.h test_ex6.c
	gcc -Wall -g -c test_ex6.c

main.o : projet.h main.c 
	gcc -Wall -g -c main.c

test_ex1 : test_ex1.o projet.o
	gcc -Wall -g -o test_ex1 test_ex1.o projet.o

test_ex2 : test_ex2.o projet.o
	gcc -Wall -g -o test_ex2 test_ex2.o projet.o

test_ex3: test_ex3.o projet.o
	gcc -Wall -g -o test_ex3 test_ex3.o projet.o

test_ex4: test_ex4.o projet.o
	gcc -Wall -g -o test_ex4 test_ex4.o projet.o

test_ex5: test_ex5.o projet.o
	gcc -Wall -g -o test_ex5 test_ex5.o projet.o


test_ex6: test_ex6.o projet.o
	gcc -Wall -g -o test_ex6 test_ex6.o projet.o


main: main.o projet.o
	gcc -Wall -g -o main main.o projet.o
clean:
	rm -f *.o test_ex1 test_ex2 test_ex3 test_ex4 test_ex5 test_ex6 main
