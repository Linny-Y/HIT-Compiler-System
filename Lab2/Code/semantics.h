#ifndef _SEMANTICS_H_
#define _SEMANTICS_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "node.h"

#define SIZE 0x3fff
#define TYPE_INT 0
#define TYPE_FLOAT 1

#define DEF_STRUCT 0
#define DEF_VAR 1

#define DEF_NOT_IN_STRUCT 0
#define DEF_IN_STRUCT 1

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct Var_* Var; 
struct Type_
{
    enum {BASIC,ARRAY,STRUCT} type;
    int basic;//0表示int 1表示float
    struct {Type elem;int size;} array;//元素类型+数组大小
    FieldList structure;//结构体类型
    
};
struct FieldList_
{
    char* name;
    Type type;
    FieldList next;
};
struct Var_{
    int is_def_struct;
    FieldList field;
    Var next;
    int line;
};
// hash function
unsigned int hash_pjw(char* name);

// check if a name is in the table
Var find_var_table(char* name);

// add a field to the table
void insert_var_table(FieldList field_list, int line, int in_struct);

// compare two types
int typecmp(Type a, Type b);
int fieldcmp(FieldList a,FieldList b, int compare_name);

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
char* OptTag(P_Node x);

// def list of variables, functions, structs (local)
FieldList DefList(P_Node now, int restr);

// def of variables, functions, structs (local)
FieldList Def(P_Node now, int restr);

// name list of variables, functions, structs (local)
FieldList DecList(P_Node now, Type type, int restr);

// name of variables, functions, structs (local)
FieldList Dec(P_Node now, Type type, int restr);

// def of variables
FieldList VarDec(P_Node now, Type type, int restr);

// paramlist of functions
FieldList VarList(P_Node now, int restr);
// param def of functions
FieldList ParamDec(P_Node now, int restr);

// // staments of functions
// void CompSt(P_Node now, Type type);
// // list of staments
// void StmtList(P_Node now, Type type);
// // stament
// void Stmt(P_Node now, Type type);
Type Exp(P_Node now);
// FieldList Args(P_Node now);


#endif