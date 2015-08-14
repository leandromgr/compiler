etapa1: lex.yy.o main.o
	gcc main.o lex.yy.o -o etapa1

main.o: main.c 
	gcc -c main.c

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c

lex.yy.c: scanner.l
	lex --header-file=lex.yy.h scanner.l

clean:
	rm -rf *.o lex* 