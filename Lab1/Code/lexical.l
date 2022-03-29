%{
    #include "syntax.tab.h"
    #include "node.h"
    int yycolumn = 1;
    extern int hasFault;
    #define YY_USER_ACTION \
        yylloc.first_line=yylloc.last_line=yylineno; \
        yylloc.first_column=yycolumn; \
        yylloc.last_column=yycolumn+yyleng-1; \
        yycolumn+=yyleng;
%}
%option yylineno
INT [1-9][0-9]*|0
FLOAT {INT}\.[0-9]+
ID [a-zA-Z_][a-zA-Z_0-9]*
SEMI ";"
COMMA ","
ASSIGNOP "="
RELOP ">"|"<"|">="|"<="|"=="|"!="
PLUS "+"
MINUS "-"
STAR "*"
DIV "/"
AND "&&"
OR "||"
DOT "."
NOT "!"
TYPE int|float
LP "("
RP ")"
LB "["
RB "]"
LC "{"
RC "}"
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
SPACE [ \t]+
ENTER [\n\r]+
%%
{SPACE} {}
{ENTER} {yycolumn = 1;}
{SEMI} {yylval.node = NewTreeNode(yylineno, "SEMI", 0); return SEMI;} 
{COMMA} {yylval.node = NewTreeNode(yylineno, "COMMA", 0); return COMMA;} 
{ASSIGNOP} {yylval.node = NewTreeNode(yylineno, "ASSIGNOP", 0); return ASSIGNOP;} 
{RELOP} {yylval.node = NewTreeNode(yylineno, "RELOP", 0); return RELOP;} 
{PLUS} { yylval.node = NewTreeNode(yylineno, "PLUS", 0); return PLUS;} 
{MINUS} {yylval.node = NewTreeNode(yylineno, "MINUS", 0); return MINUS;} 
{STAR} {yylval.node = NewTreeNode(yylineno, "STAR", 0); return STAR;} 
{DIV} {yylval.node = NewTreeNode(yylineno, "DIV", 0); return DIV;} 
{AND} {yylval.node = NewTreeNode(yylineno, "AND", 0); return AND;} 
{OR} {yylval.node = NewTreeNode(yylineno, "OR", 0); return OR;}  
{DOT} {yylval.node = NewTreeNode(yylineno, "DOT", 0); return DOT;} 
{NOT} {yylval.node = NewTreeNode(yylineno, "NOT", 0); return NOT;} 
{TYPE} {yylval.node = NewTreeNode(yylineno, "TYPE", 0); return TYPE;} 
{LP} {yylval.node = NewTreeNode(yylineno, "LP", 0); return LP;}  
{RP} {yylval.node = NewTreeNode(yylineno, "RP", 0); return RP;}  
{LB} {yylval.node = NewTreeNode(yylineno, "LB", 0); return LB;}  
{RB} {yylval.node = NewTreeNode(yylineno, "RB", 0); return RB;}  
{LC} {yylval.node = NewTreeNode(yylineno, "LC", 0); return LC;}  
{RC} {yylval.node = NewTreeNode(yylineno, "RC", 0); return RC;}  
{STRUCT} {yylval.node = NewTreeNode(yylineno, "STRUCT", 0); return STRUCT;} 
{RETURN} {yylval.node = NewTreeNode(yylineno, "RETURN", 0); return RETURN;} 
{IF} { yylval.node = NewTreeNode(yylineno, "IF", 0); return IF;} 
{ELSE} {yylval.node = NewTreeNode(yylineno, "ELSE", 0); return ELSE;} 
{WHILE} {yylval.node = NewTreeNode(yylineno, "WHILE", 0); return WHILE;} 
{INT} {yylval.node = NewTreeNode(yylineno, "INT", 0); return INT;} ; 
{FLOAT} {yylval.node = NewTreeNode(yylineno, "FLOAT", 0); return FLOAT;} 
{ID} {yylval.node = NewTreeNode(yylineno, "ID", 0); return ID;}  
{INT}+{ID}  {hasFault = TRUE; printf("Error type A at Line %d: Illegal ID \'%s\'\n", yylineno, yytext); }
\.{INT} {hasFault = TRUE; printf("Error type A at Line %d: Illegal float number \'%s\'\n", yylineno, yytext); }
. {hasFault = TRUE; printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%
//{user subroutiness}




