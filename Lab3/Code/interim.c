#include "interim.h"

extern int temp_num;
int label_num = 1;        //用于LABEL的命名
InterCode in_head = NULL; //记录双向链表的头部
InterCode in_now = NULL;  //记录现在的双向链表

Operand create_operand(enum OperandKind kind, int id, char *u_char)
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = kind;
    op->u_int = id;
    op->u_char = u_char;
    return op;
}

Operand new_temp()
{
    return create_operand(TEMP, temp_num++, "Temp");
}

Operand new_label()
{
    return create_operand(LABEL, label_num++, NULL);
}

InterCode create_intercode(enum InterCodeKind kind, int intercode_type, ...)
{
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    ic->kind = kind;
    va_list valist;
    va_start(valist, intercode_type % 10);

    if (intercode_type == INTERCODE_1)
    {
        ic->ulabel.op = va_arg(valist, Operand);
    }
    else if (intercode_type == INTERCODE_2)
    {
        ic->uassign.op1 = va_arg(valist, Operand);
        ic->uassign.op2 = va_arg(valist, Operand);
    }
    else if (intercode_type == INTERCODE_3)
    {
        ic->ubinop.result = va_arg(valist, Operand);
        ic->ubinop.op1 = va_arg(valist, Operand);
        ic->ubinop.op2 = va_arg(valist, Operand);
    }
    else if (intercode_type == INTERCODE_IF)
    {
        ic->uif.x = va_arg(valist, Operand);
        ic->uif.relop = va_arg(valist, Operand);
        ic->uif.y = va_arg(valist, Operand);
        ic->uif.z = va_arg(valist, Operand);
    }
    else if (intercode_type == INTERCODE_DEC)
    {
        ic->udec.op = va_arg(valist, Operand);
        ic->udec.size = va_arg(valist, int);
    }

    va_end(valist);
    return ic;
}

void translate_print(FILE *f)
{
    InterCode temp = in_head;

    while (temp != NULL)
    {
        if (temp->kind == ILABEL)
        {
            fprintf(f, "LABEL label%d :"), temp->ulabel.op->u_int;
        }
        else if (temp->kind == IFUNCTION)
        {
            fprintf(f, "FUNCTION %s :", temp->ulabel.op->u_char);
        }
        else if (temp->kind == ASSIGN)
        {
            Operand t1 = temp->uassign.op1;
            Operand t2 = temp->uassign.op2;
            if (t1->kind == ADDRESS && t2->kind == ADDRESS)
            {
                fprintf(f, "*t%d := *t%d", t1->u_int, t2->u_int);
            }
            else if (t1->kind == ADDRESS)
            {
                fprintf(f, "&t%d := ", t1->u_int);
                if (t2->kind == CONSTANT)
                    fprintf(f, "#%d", t2->u_int);
                else
                    fprintf(f, "t%d", t2->u_int);
            }
            else
            {
                fprintf(f, "t%d := ", t1->u_int);
                if (t2->kind == CONSTANT)
                    fprintf(f, "#%d", t2->u_int);
                else
                    fprintf(f, "t%d", t2->u_int);
            }
        }
        else if (temp->kind == ADD || temp->kind == SUB || temp->kind == MUL || temp->kind == DIV)
        {
            Operand t1 = temp->ubinop.result;
            Operand t2 = temp->ubinop.op1;
            Operand t3 = temp->ubinop.op2;
            if (t1)
            {
                fprintf(f, "t%d := ", t1->u_int);
                if (t2->kind == CONSTANT)
                    fprintf(f, "#%d", t2->u_int);
                else
                    fprintf(f, "t%d", t2->u_int);
                // operationi
                if (temp->kind == ADD)
                    fprintf(f, " + ");
                else if (temp->kind == SUB)
                    fprintf(f, " - ");
                else if (temp->kind == MUL)
                    fprintf(f, " * ");
                else if (temp->kind == DIV)
                    fprintf(f, " / ");

                if (t3->kind == CONSTANT)
                    fprintf(f, "#%d", t3->u_int);
                else
                    fprintf(f, "t%d", t3->u_int);
            }
        }
        else if (temp->kind == ADDRASS1)
        {
            Operand t1 = temp->uassign.op1;
            Operand t2 = temp->uassign.op2;
            fprintf(f, "t%d := &t%d", t1->u_int, t2->u_int);
        }
        else if (temp->kind == ADDRASS2)
        {
            Operand t1 = temp->uassign.op1;
            Operand t2 = temp->uassign.op2;
            fprintf(f, "t%d := *t%d", t1->u_int, t2->u_int);
        }
        else if (temp->kind == ADDRASS3)
        {
            Operand t1 = temp->uassign.op1;
            Operand t2 = temp->uassign.op2;
            fprintf(f, "*t%d := ", t1->u_int);
            if (t2->kind == CONSTANT)
                fprintf(f, "#%d", t2->u_int);
            else
                fprintf(f, "t%d", t2->u_int);
        }
        else if (temp->kind == GOTO)
        {
            fprintf(f, "GOTO label%d", temp->ulabel.op->u_int);
        }
        else if (temp->kind == IF)
        {
            Operand t1 = temp->uif.x;
            Operand re = temp->uif.relop;
            Operand t2 = temp->uif.y;
            Operand t3 = temp->uif.z;
            fprintf(f, "IF ");
            if (t1->kind == CONSTANT)
                fprintf(f, "#%d", t1->u_int);
            else if (t1->kind == ADDRESS)
                fprintf(f, "*t%d", t1->u_int);
            else
                fprintf(f, "t%d", t1->u_int);

            if (re->u_int == EQUAL)
                fprintf(f, " == ");
            else if (re->u_int == UNEQUAL)
                fprintf(f, " != ");
            else if (re->u_int == LESS)
                fprintf(f, " < ");
            else if (re->u_int == GREATER)
                fprintf(f, " > ");
            else if (re->u_int == LESS_EQUAL)
                fprintf(f, " <= ");
            else if (re->u_int == GREATER_EQUAL)
                fprintf(f, " >= ");

            if (t2->kind == CONSTANT)
                fprintf(f, "#%d", t2->u_int);
            else if (t2->kind == ADDRESS)
                fprintf(f, "*t%d", t2->u_int);
            else
                fprintf(f, "t%d", t2->u_int);
            fprintf(f, " GOTO label%d", t3->u_int);
        }
        else if (temp->kind == RETURN)
        {
            fprintf(f, "RETURN ");
            if (temp->ulabel.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->ulabel.op->u_int);
            else
                fprintf(f, "t%d", temp->ulabel.op->u_int);
        }
        else if (temp->kind == DEC)
        {
            fprintf(f, "DEC t%d %d", temp->udec.op->u_int, temp->udec.size);
        }
        else if (temp->kind == ARG)
        {
            fprintf(f, "ARG ");
            if (temp->ulabel.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->ulabel.op->u_int);
            else if (temp->ulabel.op->kind == ADDRESS)
                fprintf(f, "&t%d", temp->ulabel.op->u_int);
            else if (temp->ulabel.op->kind == WADDRESS)
                fprintf(f, "*t%d", temp->ulabel.op->u_int);
            else
                fprintf(f, "t%d", temp->ulabel.op->u_int);
        }
        else if (temp->kind == CALL)
        {
            Operand t1 = temp->uassign.op1;
            Operand t2 = temp->uassign.op2;
            if (t1)
                fprintf(f, "t%d := ", t1->u_int);
            else
            {
                Operand t0 = new_temp();
                fprintf(f, "t%d := ", t0->u_int);
            }
            fprintf(f, "CALL %s", t2->u_char);
        }
        else if (temp->kind == PARAM)
        {
            fprintf(f, "PARAM ");
            if (temp->ulabel.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->ulabel.op->u_int);
            else
                fprintf(f, "t%d", temp->ulabel.op->u_int);
        }
        else if (temp->kind == READ)
        {
            fprintf(f, "READ ");
            if (temp->ulabel.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->ulabel.op->u_int);
            else
                fprintf(f, "t%d", temp->ulabel.op->u_int);
        }
        else if (temp->kind == WRITE)
        {
            fprintf(f, "WRITE ");
            if (temp->ulabel.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->ulabel.op->u_int);
            else
                fprintf(f, "t%d", temp->ulabel.op->u_int);
        }
        fprintf(f, "\n");
        temp = temp->next;
    }
}

void translate_Program(P_Node now, FILE *F)
{
    // Program -> ExtDefList
    translate_ExtDefList(now->firstchild);
    translate_print(F);
}
void translate_ExtDefList(P_Node now)
{
    // ExtDefList -> ExtDef ExtDefList | empty
    if (now != NULL && now->Token != NULL){
        translate_ExtDef(now->firstchild);
        translate_ExtDefList(now->firstchild->nextbro);
    }
}

void translate_ExtDef(P_Node now)
{
    /*
    ExtDef → Specifier ExtDecList SEMI //全局变量 不考虑
    | Specifier SEMI //结构体 不考虑
    | Specifier FunDec CompSt
    */

    P_Node specifier = now->firstchild;
    if (strcmp(specifier->nextbro->Token, "SEMI") == 0 || strcmp(specifier->nextbro->Token, "ExtDecList") == 0)
    {
        return;
    }
    if (strcmp(specifier->nextbro->Token, "FunDec") == 0)
    {
        translate_FunDec(specifier->nextbro);
        translate_CompSt(specifier->nextbro->nextbro);
    }
}

void translate_FunDec(P_Node now)
{
    // FunDec → ID LP VarList RP| ID LP RP
    Operand func_op = create_operand(FUNCTION, OPERAND_NULL,now->firstchild->Id_Type);
    insert_intercode(create_intercode(IFUNCTION, INTERCODE_1, func_op));

    if (now->firstchild->nextbro->nextbro->nextbro == NULL)
        return; //ID LP RP
    
    Function find_func = find_function_table(now->firstchild->Id_Type);
    FieldList func_field = find_func->field;

    while (func_field != NULL)
    {
        Vari var = find_vari_table(func_field->name);
        Operand field_op = create_operand(VARIABLE,var->variable,func_field->name);
        insert_intercode(create_intercode(PARAM,INTERCODE_1,field_op));

        func_field = func_field->next;
    }
}

void translate_CompSt(P_Node now)
{
    // CompSt → LC DefList StmtList RC
    P_Node deflist = now->firstchild->nextbro;
    P_Node stmtlist = deflist->nextbro;
    translate_DefList(deflist);
    translate_StmtList(stmtlist);
    return;
}

void translate_DefList(P_Node now)
{
    // DefList → Def DefList| empty
    P_Node def = now->firstchild;
    while (def!=NULL && def->Token!=NULL)
    {
        translate_Def(def);
        def = def->nextbro->firstchild;
    }
}

void translate_StmtList(P_Node now)
{
    // StmtList → Stmt StmtList| empty
    P_Node child = now->firstchild;
    if (child != NULL && child->Token!=NULL)
    {
        translate_Stmt(child);
        translate_StmtList(child->nextbro);
    }
}

void translate_Def(P_Node now)
{
    // Def → Specifier DecList SEMI
    translate_DecList(now->firstchild->nextbro);
}

void translate_Stmt(P_Node now)
{
    P_Node child = now->firstchild;
    if (strcmp(child->Token, "Exp") == 0)
    {
        // Stmt → Exp SEMI
        translate_Exp(now->firstchild, NULL);
    }
    else if (strcmp(child->Token, "CompSt") == 0)
    {
        // Stmt →CompSt
        translate_CompSt(now->firstchild);
    }
    else if (strcmp(child->Token, "RETURN") == 0)
    {
        // Stmt →RETURN Exp SEMI
        Operand t1 = new_temp();
        translate_Exp(now->firstchild->nextbro, t1);
        insert_intercode(create_intercode(RETURN, INTERCODE_1, t1));
    }
    else if (strcmp(child->Token, "IF") == 0)
    {
        // Stmt →IF LP Exp RP Stmt|IF LP Exp RP Stmt ELSE Stmt
        P_Node else_node = child->nextbro->nextbro->nextbro->nextbro->nextbro;
        if (else_node == NULL)
        {
            // Stmt →IF LP Exp RP Stmt
            Operand l1 = new_label();
            Operand l2 = new_label();
            P_Node exp = child->nextbro->nextbro;
            P_Node stmt = exp->nextbro->nextbro;
            translate_Cond(exp, l1, l2);
            insert_intercode(create_intercode(ILABEL, INTERCODE_1, l1));
            translate_Stmt(stmt);
            insert_intercode(create_intercode(ILABEL, INTERCODE_1, l2));
        }
        else
        {
            // IF LP Exp RP Stmt ELSE Stmt
            Operand l1 = new_label();
            Operand l2 = new_label();
            Operand l3 = new_label();
            // code1
            P_Node exp = child->nextbro->nextbro;
            P_Node stmt = exp->nextbro->nextbro;
            P_Node else_stmt = stmt->nextbro->nextbro;

            translate_Cond(exp, l1, l2);
            insert_intercode(create_intercode(ILABEL, INTERCODE_1, l1));
            translate_Stmt(stmt);
            insert_intercode(create_intercode(GOTO, INTERCODE_1, l3));
            insert_intercode(create_intercode(ILABEL, INTERCODE_1, l2));
            translate_Stmt(else_stmt);
            insert_intercode(create_intercode(ILABEL, INTERCODE_1, l3));
        }
    }
    else if (strcmp(child->Token, "WHILE") == 0)
    {
        // Stmt →WHILE LP Exp RP Stmt
        Operand l1 = new_label();
        Operand l2 = new_label();
        Operand l3 = new_label();
        P_Node exp = child->nextbro->nextbro;
        P_Node stmt = exp->nextbro->nextbro;
        insert_intercode(create_intercode(ILABEL, INTERCODE_1, l1));
        translate_Cond(exp, l2, l3);
        insert_intercode(create_intercode(ILABEL, INTERCODE_1, l2));
        translate_Stmt(stmt);
        insert_intercode(create_intercode(GOTO, INTERCODE_1, l1));
        insert_intercode(create_intercode(ILABEL, INTERCODE_1, l3));
    }
}

void translate_DecList(P_Node now)
{
    // DecList → Dec| Dec COMMA DecList
    P_Node child = now->firstchild;
    translate_Dec(child);
    P_Node comma = child->nextbro;
    if (comma)
        translate_DecList(comma->nextbro);
}

void translate_Exp(P_Node now, Operand place)
{

    P_Node child = now->firstchild;
    P_Node child2 = child->nextbro;
    P_Node child3 = child2->nextbro;
    if (child2 == NULL)
    {
        if (place == NULL)
                return;
        if (strcmp(child->Token, "INT") == 0)
        {
            // Exp → INT
            place->kind = CONSTANT;
            place->u_int = child->intVal;
        }
        else if (strcmp(child->Token, "ID") == 0)
        {
            // Exp → ID
            Vari var = find_vari_table(child->Id_Type);
            if (var->is_from_func == FROM_FUNC && var->field->type->type != BASIC)
                place->kind = ADDRESS;
            else
                place->kind = VARIABLE;

            place->u_char = child->Id_Type;
            place->u_int = var->variable;
            place->type = var->field->type;
        }
    }
    else if (strcmp(child->Token, "MINUS") == 0)
    {
        // Exp → MINUS Exp
        if (place == NULL)
            return;
        Operand t1 = new_temp();
        translate_Exp(child->nextbro, t1);
        if (place->kind == FROM_ARG)
            place->kind = VARIABLE;
        insert_intercode(create_intercode(SUB, INTERCODE_3, place, create_operand(CONSTANT, 0, NULL), t1));
    }
    else if (strcmp(child2->Token, "ASSIGNOP") == 0)
    {
        // Exp → Exp ASSIGNOP Exp
        P_Node child_child = child->firstchild;
        Vari var = find_var_table(child_child->Token);
        Operand t1 = new_temp();
        translate_Exp(child3, t1);
        Operand t2 = create_operand(VARIABLE, var->variable, child_child->Token);
        insert_intercode(create_intercode(ASSIGN, INTERCODE_2, t2, t1));
        insert_intercode(create_intercode(ASSIGN, INTERCODE_2, place, t2));
    }
    else if (strcmp(child2->Token, "AND") == 0 || strcmp(child2->Token, "OR") == 0 || strcmp(child2->Token, "RELOP") == 0 || strcmp(child->Token, "NOT") == 0)
    {
        // Exp -> Exp AND/OR/RELOP Exp | NOT Exp
        Operand l1 = new_label(), l2 = new_label();
        insert_intercode(create_intercode(ASSIGN, INTERCODE_2, place, create_operand(CONSTANT, 0, NULL)));
        translate_Cond(now, l1, l2);
        insert_intercode(create_intercode(ILABEL, INTERCODE_1, l1));
        insert_intercode(create_intercode(ASSIGN, INTERCODE_2, place, create_operand(CONSTANT, 1, NULL)));
        insert_intercode(create_intercode(ILABEL, INTERCODE_1, l2));
    }
    else if (strcmp(child2->Token, "PLUS") == 0 || strcmp(child2->Token, "MINUS") == 0 || strcmp(child2->Token, "STAR") == 0 || strcmp(child2->Token, "DIV") == 0)
    {
        // Exp -> Exp PLUS/MINUS/STAR/DIV Exp
        Operand t1 = new_temp(), t2 = new_temp();
        translate_Exp(now->firstchild, t1);
        translate_Exp(now->firstchild->nextbro->nextbro, t2);
        Operand t3 = t1, t4 = t2;
        if (t1->kind == CONSTANT && t2->kind == CONSTANT)
            place->kind = VARIABLE;
        if (t1->kind == ADDRESS)
        {
            t3 = new_temp();
            insert_intercode(create_intercode(ADDRASS2, INTERCODE_2, t3, t1));
        }
        if (t2->kind == ADDRESS)
        {
            t4 = new_temp();
            insert_intercode(create_intercode(ADDRASS2, INTERCODE_2, t4, t2));
        }
        if (strcmp(child2->Token, "PLUS") == 0)
        {
            insert_intercode(create_intercode(ADD, INTERCODE_3, place, t3, t4));
        }
        else if (strcmp(child2->Token, "MINUS") == 0)
        {
            insert_intercode(create_intercode(SUB, INTERCODE_3, place, t3, t4));
        }
        else if (strcmp(child2->Token, "STAR") == 0)
        {
            insert_intercode(create_intercode(MUL, INTERCODE_3, place, t3, t4));
        }
        else if (strcmp(child2->Token, "DIV") == 0)
        {
            insert_intercode(create_intercode(DIV, INTERCODE_3, place, t3, t4));
        }
    }
    else if (strcmp(child2->Token, "LB") == 0)
    {
        // Exp -> Exp LB Exp RB
        if (place == NULL)
            return;
        Operand t0 = new_temp(), t1 = new_temp(), t2 = new_temp(), t3 = new_temp();
        t0->kind = ADDRESS;
        translate_Exp(child, t1);
        int offset = get_offset(t1->type);
        insert_intercode(create_intercode(ADDRASS1, INTERCODE_2, t0, t1));

        translate_Exp(child3, t3);
        insert_intercode(create_intercode(MUL, INTERCODE_3, t2, t3, create_operand(CONSTANT, offset, NULL)));
        if (place->kind == FROM_ARG)
            place->kind = WADDRESS;
        else
            place->kind = ADDRESS;
        place->type = t1->type->array.elem;
        insert_intercode(create_intercode(ADD, INTERCODE_3, place, t0, t2));
    }
    else if (strcmp(child2->Token, "Exp") == 0)
    {
        // Exp → LP Exp RP
        translate_Exp(child2, place);
    }
    else if (strcmp(child3->Token, "Args") == 0)
    {
        // Exp → ID LP Args RP
        //函数调用，有参数
        if (strcmp(child->Id_Type, "write") == 0)
        {
            Operand t1 = new_temp();
            translate_Exp(child3->firstchild, t1);
            //要对地址类的取地址处理
            if (t1->kind == ADDRESS)
            {
                Operand t0 = new_temp();
                insert_intercode(create_intercode(ADDRASS2, INTERCODE_2, t0, t1));
                insert_intercode(create_intercode(WRITE, INTERCODE_1, t0));
            }
            else
                insert_intercode(create_intercode(WRITE, INTERCODE_1, t1));
            return;
        }
        translate_Args(child3, NULL);
        Operand func_op = create_operand(FUNCTION,OPERAND_NULL,child->Id_Type);
        insert_intercode(create_intercode(CALL, INTERCODE_2, place, func_op));
        return;
    }
    else if (strcmp(child3->Token, "RP") == 0)
    {
        // Exp → ID LP RP
        //没有参数的函数调用
        if (strcmp(child->Id_Type, "read") == 0)
        {
            insert_intercode(create_intercode(READ, INTERCODE_1, place));
            return;
        }
        Operand func_op = create_operand(FUNCTION,OPERAND_NULL,child->Id_Type);
        insert_intercode(create_intercode(CALL, INTERCODE_2, place, func_op));
        return;
    }
}

void translate_Cond(P_Node now, Operand lt, Operand lf)
{
    P_Node child = now->firstchild;

    if (child->nextbro != NULL && strcmp(child->nextbro->Token, "RELOP") == 0)
    {
        // printf("a translate_Cond RELOP\n");
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        // code1+code2
        translate_Exp(child, t1);
        translate_Exp(child->nextbro->nextbro, t2);
        Operand op = (Operand)malloc(sizeof(struct Operand_));
        op->kind = RELOP;
        op->u_int = child->nextbro->line;
        // code3
        InterCode code3 = (InterCode)malloc(sizeof(struct InterCode_));
        code3->kind = IF;
        code3->uif.x = t1;
        code3->uif.relop = op;
        code3->uif.y = t2;
        code3->uif.z = lt;
        insert_intercode(code3);
        // goto labelfalse
        InterCode code4 = (InterCode)malloc(sizeof(struct InterCode_));
        code4->kind = GOTO;
        code4->ulabel.op = lf;
        insert_intercode(code4);
    }
    else if (strcmp(child->Token, "NOT") == 0)
    {
        // printf("a translate_Cond NOT\n");
        translate_Cond(now->firstchild->nextbro, lf, lt);
    }
    else if (child->nextbro != NULL && strcmp(child->nextbro->Token, "AND") == 0)
    {
        // printf("a translate_Cond AND\n");
        Operand l1 = new_label();
        // code1
        translate_Cond(child, l1, lf);
        // label1
        InterCode code1 = (InterCode)malloc(sizeof(struct InterCode_));
        code1->kind = ILABEL;
        code1->ulabel.op = l1;
        insert_intercode(code1);
        // code2
        translate_Cond(child->nextbro->nextbro, lt, lf);
    }
    else if (child->nextbro != NULL && strcmp(child->nextbro->Token, "OR") == 0)
    {
        // printf("a translate_Cond OR\n");
        Operand l1 = new_label();
        // code1
        translate_Cond(child, lt, l1);
        // label1
        InterCode code1 = (InterCode)malloc(sizeof(struct InterCode_));
        code1->kind = ILABEL;
        code1->ulabel.op = l1;
        insert_intercode(code1);
        // code2
        translate_Cond(child->nextbro->nextbro, lt, lf);
        // printf("a ORout\n");
    }
    else
    {
        // printf("a translate_Cond else\n");
        Operand t1 = new_temp();
        // code1
        translate_Exp(now, t1);
        // code2
        Operand op = (Operand)malloc(sizeof(struct Operand_));
        op->kind = RELOP;
        op->u_int = 1;
        Operand t2 = (Operand)malloc(sizeof(struct Operand_));
        t2->kind = CONSTANT;
        t2->u_int = 0;
        // printf("a out0\n");
        InterCode code2 = (InterCode)malloc(sizeof(struct InterCode_));
        code2->kind = IF;
        code2->uif.x = t1;
        code2->uif.relop = op;
        code2->uif.y = t2;
        code2->uif.z = lt;
        insert_intercode(code2);
        // goto
        InterCode code4 = (InterCode)malloc(sizeof(struct InterCode_));
        code4->kind = GOTO;
        code4->ulabel.op = lf;
        insert_intercode(code4);
        // printf("a out1\n");
    }
    return;
}

void translate_Dec(P_Node now)
{
    // Dec → VarDec| VarDec ASSIGNOP Exp
    P_Node vardec = now->firstchild;
    P_Node child2 = vardec->nextbro;
    if (child2 == NULL)
    {
        Operand t1 = new_temp();
        translate_VarDec(vardec, t1);
    }
    else
    {
        Operand rem1 = new_temp();
        Operand rem2 = new_temp();
        translate_VarDec(vardec, rem1);
        translate_Exp(child2->nextbro, rem2);

        InterCode func_in = (InterCode)malloc(sizeof(struct InterCode_));
        func_in->kind = ASSIGN;
        func_in->uassign.op1 = rem1;
        func_in->uassign.op2 = rem2;
        insert_intercode(func_in);
    }
}

void translate_VarDec(P_Node now, Operand place)
{
    // VarDec → ID| VarDec LB INT RB
    //会生成DEC语句
    //只会出现一维数组，所以在else语句中直接处理数组
    // printf("a translate_VarDec\n");
    if (now->firstchild->nextbro == NULL)
    {
        //可能是普通变量，可能是structure
        Vari find_it = find_vari_table(now->firstchild->Id_Type);
        // printf("a is_find?%d\n",find_it);
        FieldList find_field = find_it->field;
        if (find_field->type->type == BASIC)
        {
            // printf("a isBASIC\n");
            // printf("a %s\n",now->firstchild->Id_Type);
            if (place == NULL)
                return;
            place->kind = VARIABLE;
            place->u_int = find_it->variable;
            place->u_char = now->firstchild->Id_Type;
            return;
        }
        else if (find_field->type->type == STRUCT)
        {
            // printf("a isSTRUCT\n");
            // printf("a %s\n",now->firstchild->Id_Type);
            if (place == NULL)
                return;
            place->kind = VARIABLE;
            place->u_int = find_it->variable;
            place->u_char = now->firstchild->Id_Type;
            //生成dec语句
            // printf("a create_dec3\n");
            InterCode func_in = (InterCode)malloc(sizeof(struct InterCode_));
            func_in->kind = DEC;
            func_in->udec.op = place;
            func_in->udec.size = get_size(find_field->type);
            insert_intercode(func_in);
            return;
        }
        // printf("a isERROR\n");
    }
    else
    {
        
        // printf("a isARRAY\n");
        if (place == NULL)
            return;
        place->kind = VARIABLE;
        place->u_char = now->firstchild->Id_Type;
        Vari find_it = find_vari_table(now->firstchild->firstchild->Id_Type);
        place->u_int = find_it->variable;
        //生成DEC语句
        // printf("a create_dec2\n");
        InterCode func_in = (InterCode)malloc(sizeof(struct InterCode_));
        func_in->kind = DEC;
        func_in->udec.op = place;
        func_in->udec.size = get_size(find_it->field->type);
        insert_intercode(func_in);
        return;
    }
}

void translate_Args(P_Node now, InterCode here)
{
    //传递的如果是数组、结构体，是一个address类型
    //逆序
    // Args → Exp COMMA Args| Exp
    P_Node exp = now->firstchild;
    // 1.翻译exp信息
    Operand t1 = new_temp();
    t1->kind = FROM_ARG;
    translate_Exp(exp, t1);
    // 1.0修改kind类型
    if (t1->kind == CONSTANT || t1->kind == ADDRESS || t1->kind == WADDRESS)
        ;
    else
    {
        Vari find_id = find_vari_table(t1->u_char);
        if (find_id == NULL)
            t1->kind = VARIABLE;
        else if (find_id->field->type->type != BASIC)
            t1->kind = ADDRESS;
        else
            t1->kind = VARIABLE;
    }

    // 2.生成ARG语句
    InterCode code1_in = (InterCode)malloc(sizeof(struct InterCode_));
    code1_in->kind = ARG;
    code1_in->ulabel.op = t1;
    // 3.连接
    if (here == NULL)
    {
        code1_in->next = NULL;
        code1_in->prev = NULL;
    }
    else
    {
        here->next = code1_in;
        code1_in->prev = here;
    }
    exp = exp->nextbro;
    if (exp == NULL)
        ;
    else
        translate_Args(exp->nextbro, code1_in);
    if (here == NULL)
    {
        InterCode temp = code1_in;
        while (temp->next != NULL)
            temp = temp->next;
        while (temp != NULL)
        {
            InterCode rem = temp;
            temp = temp->prev;
            insert_intercode(rem);
        }
    }
}

void translate_StructSpecifier(P_Node now)
{
    // StructSpecifier → STRUCT OptTag LC DefList RC| STRUCT Tag
    P_Node struct_s = now->firstchild;
    P_Node child2 = struct_s->nextbro;
    P_Node child3 = child2->nextbro;
    if (child3 == NULL)
    {
        // Tag → ID,生成DEC语句
        Operand t1 = (Operand)malloc(sizeof(struct Operand_));
        Vari find = find_vari_table(child2->firstchild->Id_Type);
        t1->kind = VARIABLE;
        t1->u_int = find->variable;
        t1->u_char = child2->firstchild->Id_Type;
        // printf("a create_dec1\n");
        InterCode func_in = (InterCode)malloc(sizeof(struct InterCode_));
        func_in->kind = DEC;
        func_in->udec.op = t1;
        func_in->udec.size = get_size(find->field->type);
        insert_intercode(func_in);
        return;
    }
}

void insert_intercode(InterCode this)
{
    this->prev = NULL;
    this->next = NULL;
    if (in_head == NULL)
    {
        in_head = this;
    }
    if (in_now == NULL)
        in_now = this;
    else
    {
        in_now->next = this;
        this->prev = in_now;
        in_now = this;
    }
}

//计算部分偏移
int get_offset(Type return_type)
{
    if (return_type == NULL)
        return 0;
    if (return_type->type == BASIC)
        return 4;
    else if (return_type->type == ARRAY)
    {
        if (return_type->array.elem->type == ARRAY)
        {
            // interim_error = 1;
            printf("Cannot translate: Code contains variables of error array type.\n");
            return -1;
        }
        return get_size(return_type->array.elem);
    }
}

//计算全部偏移，即type中的所有内存都要加上
int get_size(Type type)
{
    if (type == NULL)
        return 0;
    if (type->type == BASIC)
        return 4;
    else if (type->type == ARRAY)
    {
        int temp = type->array.size * get_size(type->array.elem);
        return temp;
    }
}
