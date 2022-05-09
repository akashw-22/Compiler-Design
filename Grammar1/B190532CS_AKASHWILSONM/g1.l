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


[+*-><=();{}]           {return *yytext;}

">="                    {return GE;}
"<="                    {return LE;}
"=="                    {return EE;}
"!="                    {return NE;}

[rR][eE][Aa][Dd]        {return READ;}

[wW][rR][iI][tT][eE]    {return WRITE;}

[iI][fF]                {return IF;}

[eE][lL][sS][Ee]        {return ELSE;}

[Ww][hH][iI][lL][eE]    {return WHILE;}

[dD][oO]                {return DO;}

[fF][uU][nN]            {return FUN;}

[a-zA-Z][a-zA-Z0-9]*    {
                            yylval.node = makeID(yytext);
                            return VAR;
                        }

[ \t\n]                 {;}

.                       {
                            printf("unexpected character..\n");
                        }

%%
