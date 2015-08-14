etapa1: lex.yy.o hash.o main.o
	gcc main.o lex.yy.o hash.o -o etapa1

main.o: main.c 
	gcc -c main.c

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c

lex.yy.c: scanner.l
	lex --header-file=lex.yy.h scanner.l

hash.o: hash.c
	gcc -c hash.c

clean:
	rm -rf *.o lex* 
