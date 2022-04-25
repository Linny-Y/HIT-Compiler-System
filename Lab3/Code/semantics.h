#ifndef _SEMANTICS_H_
#define _SEMANTICS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "node.h"

#define SIZE 0x3fff
#define T_INT 0
#define T_FLOAT 1

#define DEF_STRUCT 0
#define DEF_VAR 1
#define DEF_FUNC 2

#define DEF_NOT_IN_STRUCT 0
#define DEF_IN_STRUCT 1

#define FROM_FUNC 0
#define NOT_FROM_FUNC 1

typedef struct Type_ *Type;
typedef struct FieldList_ *FieldList;
typedef struct Vari_ *Vari;
typedef struct Function_ *Function;
struct Type_
{
    enum
    {
        BASIC,
        ARRAY,
        STRUCT
    } type;
    int basic; // 0表示int 1表示float
    struct
    {
        Type elem;
        int size;
    } array;             //元素类型+数组大小
    FieldList structure; //结构体类型
};
struct FieldList_
{
    char *name;
    Type type;
    FieldList next;
};
struct Vari_
{
    int is_def_struct;
    FieldList field;
    Vari next;
    int line;
    int variable;//用于lab3的变量,记录其t后的数据
    int is_from_func;//用于lab3的variable以及address类型的区分
};
struct Function_
{
    char *name;
    FieldList field;
    Type return_type;
    int defined;
    int line;
    Function next;
};

// hash function
unsigned int hash_pjw(char *name);

// check if a name is in the table
Vari find_vari_table(char *name);

// add a field to the table
void insert_vari_table(FieldList field_list, int line, int in_struct,int is_from_fun);

// compare two types
int typecmp(Type a, Type b);
int fieldcmp(FieldList a, FieldList b, int compare_name);

Function find_function_table(char *name);
// add a function to the function table
void insert_function_table(Function func);

// compare two functions
int funccmp(Function a, Function b);

// -----------------semantic analysis------------------------
// ---------------------program---------------------------
void semantic_analysis(P_Node now);

// def of global variables, functions, structs
void ExtDef(P_Node now);
// def of global variables
void ExtDecList(P_Node now, Type type);
// description of type, including TYPE and struct
Type Specifier(P_Node now);
// description of struct
Type StructSpecifier(P_Node now);
// name of struct
char *OptTag(P_Node now);

// def list of variables, functions, structs (local)
FieldList DefList(P_Node now, int judge);

// def of variables, functions, structs (local)
FieldList Def(P_Node now, int judge);

// name list of variables, functions, structs (local)
FieldList DecList(P_Node now, Type type, int judge);

// name of variables, functions, structs (local)
FieldList Dec(P_Node now, Type type, int judge);

// def of variables
FieldList VarDec(P_Node now, Type type, int judge);

void FunDec(P_Node now, Type type, int judge);
// paramlist of functions
FieldList VarList(P_Node now, int judge);
// param def of functions
FieldList ParamDec(P_Node now, int judge);

// statements of functions
void CompSt(P_Node now, Type type);
// list of statements
void StmtList(P_Node now, Type type);
// statement
void Stmt(P_Node now, Type type);
Type Exp(P_Node now);
FieldList Args(P_Node now);

#endif