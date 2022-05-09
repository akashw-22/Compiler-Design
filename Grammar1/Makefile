test: label g1.l g1.y lex.yy.c y.tab.c exptree.c exptree.h g1.l g1.y
	lex g1.l
	yacc -d g1.y
	gcc lex.yy.c y.tab.c -o output
	./output < $(input)

lex.yy.c:
	lex g1.l

y.tab.c:
	yacc -d g1.y

clean:
	rm lex.yy.c y.tab.c y.tab.h output *.xsm

label: