#****INSTITUTO DE INFORMATICA UFRGS****
#Leandro Mateus Giacomini Rocha
#Luciano Farias Puhl

DEBUG=

etapa4: y.tab.o semantic.o lex.yy.o main.o
	gcc $(DEBUG) main.o lex.yy.o hash.o ast.o semantic.o y.tab.o -o etapa4

main.o: main.c 
	gcc $(DEBUG) -c main.c

y.tab.o: y.tab.c hash.o ast.o 
	gcc $(DEBUG) -c y.tab.c

y.tab.c: parser.y
	yacc -d parser.y

lex.yy.o: lex.yy.c
	gcc $(DEBUG) -c lex.yy.c

lex.yy.c: scanner.l
	lex --header-file=lex.yy.h scanner.l

hash.o: hash.c
	gcc $(DEBUG) -c hash.c

ast.o: ast.c
	gcc $(DEBUG) -c ast.c

semantic.o: semantic.c
	gcc $(DEBUG) -c semantic.c

tar:
	tar -czvf etapa4.tgz Makefile hash.c hash.h scanner.l parser.y main.c ast.h ast.c semantic.c semantic.h

clean:
	rm -rf *.o lex* y.tab* etapa[1-9] *.tgz
