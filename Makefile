#****INSTITUTO DE INFORMATICA UFRGS****
#Leandro Mateus Giacomini Rocha
#Luciano Farias Puhl

etapa2: y.tab.o lex.yy.o hash.o main.o
	gcc main.o lex.yy.o hash.o y.tab.o -o etapa2

main.o: main.c 
	gcc -c main.c

y.tab.o: y.tab.c
	gcc -c y.tab.c

y.tab.c: parser.y
	yacc -d -Wall parser.y

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c

lex.yy.c: scanner.l
	lex --header-file=lex.yy.h scanner.l

hash.o: hash.c
	gcc -c hash.c

clean:
	rm -rf *.o lex* y.tab* etapa[1-9]
