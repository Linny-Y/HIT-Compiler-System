#ifndef INTERIM_H
#define INTERIM_H
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "semantics.h"

#define INTERCODE_1 1
#define INTERCODE_2 2
#define INTERCODE_3 3
#define INTERCODE_IF 4
#define INTERCODE_DEC 12
#define OPERAND_NULL -1

typedef struct Operand_ *Operand;
typedef struct InterCode_ *InterCode;

enum OperandKind
{
    VARIABLE,
    CONSTANT,
    ADDRESS,
    FUNCTION,
    LABEL,
    RELOP,
    UNKNOWN
};
struct Operand_
{
    enum OperandKind kind;
    int u_int;
    char *u_char;
    Type type;
};

enum InterCodeKind
{
    ILABEL,
    IFUNCTION,
    ASSIGN,
    ADD,
    SUB,
    MUL,
    DIV,
    ADDRASS1,
    ADDRASS2,
    ADDRASS3,
    GOTO,
    IF,
    RETURN,
    DEC,
    ARG,
    CALL,
    PARAM,
    READ,
    WRITE
};
struct InterCode_
{
    enum InterCodeKind kind;
    union
    {
        // LABEL, FUNCTION, GOTO, RETURN, ARG, PARAM, READ, WRITE
        struct
        {
            Operand op;
        } operand_1;
        // ASSIGN, CALL, ADDRASS1, ADDRASS2, ADDRASS3
        struct
        {
            Operand op1, op2;
        } operand_2;
        // ADD, SUB, MUL, DIV
        struct
        {
            Operand result, op1, op2;
        } operand_3;
        // IF
        struct
        {
            Operand x, relop, y, z;
        } operand_if;
        // DEC
        struct
        {
            Operand op;
            int size;
        } operand_dec;
    };
    InterCode prev;
    InterCode next;
};

Operand new_temp();
Operand new_label();
void translate_print(FILE *f);

void translate_Program(P_Node now, FILE *F);
void translate_ExtDefList(P_Node now);
void translate_ExtDef(P_Node now);
void translate_FunDec(P_Node now);
void translate_CompSt(P_Node now);
void translate_DefList(P_Node now);
void translate_StmtList(P_Node now);
void translate_Def(P_Node now);
void translate_Stmt(P_Node now);
void translate_DecList(P_Node now);
void translate_Exp(P_Node now, Operand place);
void translate_CompSt(P_Node now);
void translate_Cond(P_Node now, Operand lt, Operand lf);
void translate_Dec(P_Node now);
void translate_VarDec(P_Node now, Operand place);
void translate_Args(P_Node now, InterCode here);

void insert_intercode(InterCode this);
int get_offset(Type return_type);
int get_size(Type type);

#endif