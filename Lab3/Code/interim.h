#ifndef INTERIM_H
#define INTERIM_H
#include <stdio.h>
#include <stdlib.h>
#include "semantics.h"
//优化
// 1.t1=*a, t1=1 ==> *a=1
// 2.0赋值多次

#define EQUAL 0
#define UNEQUAL 1
#define LESS 2
#define GREATER 3
#define LESS_EQUAL 4
#define GREATER_EQUAL 5

typedef struct Operand_ *Operand;
typedef struct InterCode_ *InterCode;

struct Operand_
{
    enum
    {
        FROM_ARG, // FROM_ARG用于传递给exp的arg，因为做为函数参数时的temp, temp[2]都会返回address//但实际上，temp[2]应返回basic
        VARIABLE, // VARIABLE普通函数参数, char+int//先查询符号表中是否以及给该变量var值
        TEMP,     // TEMP临时变量, int
        CONSTANT, // CONSTANT常数，int
        ADDRESS,  // ADDRESS结构体、数组做为函数参数char+int
        WADDRESS, // WADDRESS写入地址
        FUNCTION, // FUNCTION函数, char
        LABEL,    // LABEL标号
        RELOP     // RELOP比较，见下, int//相等0，不等1，小于2，大于3，小于等于4，大于等于5
    } kind;
    int u_int; // t1t2
    char *u_char;
    Type type; //用于结构体和数组变量的Offset查询
};

// PARAM打印param v+u_int
// ASSIGN打印：注意立即数
// ADDRASS2打印：t2=*t1
// ADDRADD3其实是在赋值语句中，如果左为address, 右为其他的话
// ARG打印：如果address就&，不是则正常
struct InterCode_
{
    enum
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
    } kind;
    union
    {
        // LABEL, FUNCTION, GOTO, RETURN, ARG
        // PARAM, READ, WRITE
        struct
        {
            Operand op;
        } ulabel;
        // ASSIGN, CALL
        // ADDRASS1, ADDRASS2, ADDRASS3
        struct
        {
            Operand op1, op2;
        } uassign;
        // ADD, SUB, MUL, DIV
        struct
        {
            Operand result, op1, op2;
        } ubinop;
        // IF
        struct
        {
            Operand x, relop, y, z;
        } uif;
        // DEC
        struct
        {
            Operand op;
            int size;
        } udec;
    };
    InterCode prev;
    InterCode next;
};

void translate_print_test(InterCode temp);
void translate_print(FILE *f);

void translate_Program(P_Node now, FILE *F);
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
void translate_Specifier(P_Node now);
void translate_StructSpecifier(P_Node now);

void add_to_intercode(InterCode this);
Operand new_temp();
Operand new_label();
// t0=0表示从结构体中来，t0=1表示从数组中来
int get_offset(Type return_type, P_Node after);
int get_size(Type type);

#endif