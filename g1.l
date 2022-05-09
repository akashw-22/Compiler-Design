%option noyywrap
%{
    #include <stdlib.h>
    #include <stdio.h>
    #include "y.tab.h"
    #include "exptree.h"

    void printinlex(char token[100]);
    int num;
%}

%%



[0-9]+                  { 
                            num = atoi(yytext);
                            yylval.node = makeLeaf(num);
                            return NUM;
                        }


[+*><=();{}]            {return *yytext;}

">="                    {return GE;}
"<="                    {return LE;}
"=="                    {return EE;}
"!="                    {return NE;}

"read"                  {return READ;}

"write"                 {return WRITE;}

"If"                    {return IF;}

"else"                  {return ELSE;}

"while"                 {return WHILE;}

"do"                    {return DO;}

"FUN"                 {return FUN;}

[a-zA-Z][a-zA-Z0-9]*    {
                            yylval.node = makeID(yytext);
                            return ID;
                        }

.                       

%%
