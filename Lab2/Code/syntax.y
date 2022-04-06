%{
    #include "lex.yy.c"
    #include "node.h"
    int yyerror(char* msg);
    extern int hasFault;
    P_Node root;
%}
/* declared types */
%union {
    struct treeNode* node;
}
/* declared tokens */
%token <node> INT FLOAT ID
%token <node> SEMI COMMA
%token <node> ASSIGNOP RELOP 
%token <node> PLUS MINUS STAR DIV
%token <node> AND OR DOT NOT
%token <node> LP RP LB RB LC RC
%token <node> TYPE STRUCT RETURN
%token <node> IF ELSE WHILE
//%token SPACE ENTER

// non-terminals
%type <node> Program ExtDefList ExtDef ExtDecList   //  High-level Definitions
%type <node> Specifier StructSpecifier OptTag Tag   //  Specifiers
%type <node> VarDec FunDec VarList ParamDec         //  Declarators
%type <node> CompSt StmtList Stmt                   //  Statements
%type <node> DefList Def Dec DecList                //  Local Definitions
%type <node> Exp Args                               //  Expressions

/* Priority and Associativity */
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT
%nonassoc ELSE
%%
/* High-level Definitions */
Program : ExtDefList {$$ = NewTreeNode(@$.first_line, "Program", 1, $1); root = $$;} /* 定义根节点 */
    ;
ExtDefList : ExtDef ExtDefList {$$ = NewTreeNode(@$.first_line, "ExtDefList", 2, $1, $2);}
    | /* empty */ {$$ = EmptyNode();}
    ;
ExtDef : Specifier ExtDecList SEMI {$$ = NewTreeNode(@$.first_line, "ExtDef", 3, $1, $2, $3);}
    | Specifier SEMI {$$ = NewTreeNode(@$.first_line, "ExtDef", 2, $1, $2);}
    | Specifier FunDec CompSt {$$ = NewTreeNode(@$.first_line, "ExtDef", 3, $1, $2, $3);}
    | error SEMI {hasFault = TRUE;}
    ;
ExtDecList : VarDec {$$ = NewTreeNode(@$.first_line, "ExtDecList", 1, $1);}
    | VarDec COMMA ExtDecList {$$ = NewTreeNode(@$.first_line, "ExtDecList", 3, $1, $2, $3);}
    ;

/* Specifiers */
Specifier : TYPE {$$ = NewTreeNode(@$.first_line, "Specifier", 1, $1);}
    | StructSpecifier {$$ = NewTreeNode(@$.first_line, "Specifier", 1, $1);}
    ;
StructSpecifier : STRUCT OptTag LC DefList RC {$$ = NewTreeNode(@$.first_line, "StructSpecifier", 5, $1, $2, $3, $4, $5);}
    | STRUCT Tag {$$ = NewTreeNode(@$.first_line, "StructSpecifier", 2, $1, $2);}
    | error RC {hasFault = TRUE;}
    ;
OptTag : ID {$$ = NewTreeNode(@$.first_line, "OptTag", 1, $1);}
    | /* empty */ {$$ = EmptyNode();}
    ;
Tag : ID {$$ = NewTreeNode(@$.first_line, "Tag", 1, $1);}
    ;

/* Declarators */
VarDec : ID {$$ = NewTreeNode(@$.first_line, "VarDec", 1, $1);}
    | VarDec LB INT RB {$$ = NewTreeNode(@$.first_line, "VarDec", 4, $1, $2, $3, $4);}
    | error RB {hasFault = TRUE;}
    ;
FunDec : ID LP VarList RP {$$ = NewTreeNode(@$.first_line, "FunDec", 4, $1, $2, $3, $4);}
    | ID LP RP {$$ = NewTreeNode(@$.first_line, "FunDec", 3, $1, $2, $3);}
    | error RP {hasFault = TRUE;}
    ;
VarList : ParamDec COMMA VarList {$$ = NewTreeNode(@$.first_line, "VarList", 3, $1, $2, $3);}
    | ParamDec {$$ = NewTreeNode(@$.first_line, "VarList", 1, $1);}
    ;
ParamDec : Specifier VarDec {$$ = NewTreeNode(@$.first_line, "ParamDec", 2, $1, $2);}
    ;

/* Statements */
CompSt : LC DefList StmtList RC {$$ = NewTreeNode(@$.first_line, "CompSt", 4, $1, $2, $3, $4);}
    | error RC {hasFault = TRUE;}
    ;
StmtList : Stmt StmtList {$$ = NewTreeNode(@$.first_line, "StmtList", 2, $1, $2);}
    | /* empty */ {$$ = EmptyNode();}
    ;
Stmt : Exp SEMI {$$ = NewTreeNode(@$.first_line, "Stmt", 2, $1, $2);}
    | CompSt {$$ = NewTreeNode(@$.first_line, "Stmt", 1, $1);}
    | RETURN Exp SEMI {$$ = NewTreeNode(@$.first_line, "Stmt", 3, $1, $2, $3);}
    | IF LP Exp RP Stmt {$$ = NewTreeNode(@$.first_line, "Stmt", 5, $1, $2, $3, $4, $5);}
    | IF LP Exp RP Stmt ELSE Stmt {$$ = NewTreeNode(@$.first_line, "Stmt", 7, $1, $2, $3, $4, $5, $6, $7);}
    | WHILE LP Exp RP Stmt {$$ = NewTreeNode(@$.first_line, "Stmt", 5, $1, $2, $3, $4, $5);}
    | error SEMI {hasFault = TRUE;}
    ;

/* Local Definitions */
DefList : Def DefList {$$ = NewTreeNode(@$.first_line, "DefList", 2, $1, $2);}
    | /* empty */ {$$ = EmptyNode();}
    ;
Def : Specifier DecList SEMI {$$ = NewTreeNode(@$.first_line, "Def", 3, $1, $2, $3);}
    | error SEMI {hasFault = TRUE;}
    ;
DecList : Dec {$$ = NewTreeNode(@$.first_line, "DecList", 1, $1);}
    | Dec COMMA DecList {$$ = NewTreeNode(@$.first_line, "DecList", 3, $1, $2, $3);}
    ;
Dec : VarDec {$$ = NewTreeNode(@$.first_line, "Dec", 1, $1);}
    | VarDec ASSIGNOP Exp {$$ = NewTreeNode(@$.first_line, "Dec", 3, $1, $2, $3);}
    ;

/* Expressions */
Exp : Exp ASSIGNOP Exp {$$ = NewTreeNode(@$.first_line, "Exp", 3, $1, $2, $3);}
    | Exp AND Exp {$$ = NewTreeNode(@$.first_line, "Exp", 3, $1, $2, $3);}
    | Exp OR Exp {$$ = NewTreeNode(@$.first_line, "Exp", 3, $1, $2, $3);}
    | Exp RELOP Exp {$$ = NewTreeNode(@$.first_line, "Exp", 3, $1, $2, $3);}
    | Exp PLUS Exp {$$ = NewTreeNode(@$.first_line, "Exp", 3, $1, $2, $3);}
    | Exp MINUS Exp {$$ = NewTreeNode(@$.first_line, "Exp", 3, $1, $2, $3);}
    | Exp STAR Exp {$$ = NewTreeNode(@$.first_line, "Exp", 3, $1, $2, $3);}
    | Exp DIV Exp {$$ = NewTreeNode(@$.first_line, "Exp", 3, $1, $2, $3);}
    | LP Exp RP {$$ = NewTreeNode(@$.first_line, "Exp", 3, $1, $2, $3);}
    | MINUS Exp {$$ = NewTreeNode(@$.first_line, "Exp", 2, $1, $2);}
    | NOT Exp {$$ = NewTreeNode(@$.first_line, "Exp", 2, $1, $2);}
    | ID LP Args RP {$$ = NewTreeNode(@$.first_line, "Exp", 4, $1, $2, $3, $4);}
    | ID LP RP {$$ = NewTreeNode(@$.first_line, "Exp", 3, $1, $2, $3);}
    | Exp LB Exp RB {$$ = NewTreeNode(@$.first_line, "Exp", 4, $1, $2, $3, $4);}
    | Exp DOT ID {$$ = NewTreeNode(@$.first_line, "Exp", 3, $1, $2, $3);}
    | ID {$$ = NewTreeNode(@$.first_line, "Exp", 1, $1);}
    | INT {$$ = NewTreeNode(@$.first_line, "Exp", 1, $1);}
    | FLOAT {$$ = NewTreeNode(@$.first_line, "Exp", 1, $1);}
    //| error RP {hasFault = TRUE;}
    //| error RB {hasFault = TRUE;}
    ;
Args : Exp COMMA Args {$$ = NewTreeNode(@$.first_line, "Args", 3, $1, $2, $3);}
    | Exp {$$ = NewTreeNode(@$.first_line, "Args", 1, $1);}
    ;



%%
int yyerror(char* msg){
    fprintf(stderr, "Error type B at Line %d: %s, near\'%s\'\n", yylineno, msg, yytext);
}
