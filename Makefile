#****INSTITUTO DE INFORMATICA UFRGS****
#Leandro Mateus Giacomini Rocha
#Luciano Farias Puhl

etapa3: y.tab.o semantic.o lex.yy.o main.o
	gcc main.o lex.yy.o hash.o ast.o semantic.o y.tab.o -o etapa4

main.o: main.c 
	gcc -c main.c

y.tab.o: y.tab.c hash.o ast.o 
	gcc -c y.tab.c

y.tab.c: parser.y
	yacc -d parser.y

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c

lex.yy.c: scanner.l
	lex --header-file=lex.yy.h scanner.l

hash.o: hash.c
	gcc -c hash.c

ast.o: ast.c
	gcc -c ast.c

semantic.o: semantic.c
	gcc -c semantic.c

clean:
	rm -rf *.o lex* y.tab* etapa[1-9]
