#include "interim.h"

extern int temp_num;
int label_num = 1;        //用于LABEL的命名
InterCode in_head = NULL; //记录双向链表的头部
InterCode in_now = NULL;  //记录现在的双向链表的指针位置·

Operand create_operand(enum OperandKind kind, int id, char *u_char)
{
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = kind;
    op->u_int = id;
    op->u_char = u_char;
    op->type = NULL;
    return op;
}

InterCode create_intercode(enum InterCodeKind kind, int intercode_type, ...)
{
    InterCode ic = (InterCode)malloc(sizeof(struct InterCode_));
    ic->kind = kind;
    va_list valist;
    va_start(valist, (intercode_type % 10));

    if (intercode_type == INTERCODE_1)
    {
        ic->operand_1.op = va_arg(valist, Operand);
    }
    else if (intercode_type == INTERCODE_2)
    {
        ic->operand_2.op1 = va_arg(valist, Operand);
        ic->operand_2.op2 = va_arg(valist, Operand);
    }
    else if (intercode_type == INTERCODE_3)
    {
        ic->operand_3.result = va_arg(valist, Operand);
        ic->operand_3.op1 = va_arg(valist, Operand);
        ic->operand_3.op2 = va_arg(valist, Operand);
    }
    else if (intercode_type == INTERCODE_IF)
    {
        ic->operand_if.x = va_arg(valist, Operand);
        ic->operand_if.relop = va_arg(valist, Operand);
        ic->operand_if.y = va_arg(valist, Operand);
        ic->operand_if.z = va_arg(valist, Operand);
    }
    else if (intercode_type == INTERCODE_DEC)
    {
        ic->operand_dec.op = va_arg(valist, Operand);
        ic->operand_dec.size = va_arg(valist, int);
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
            fprintf(f, "LABEL label%d :", temp->operand_1.op->u_int);
        }
        else if (temp->kind == IFUNCTION)
        {
            fprintf(f, "FUNCTION %s :", temp->operand_1.op->u_char);
        }
        else if (temp->kind == ASSIGN)
        {
            Operand t1 = temp->operand_2.op1;
            Operand t2 = temp->operand_2.op2;
            if (t1 == NULL || t2 == NULL) ;
            else if (t1->kind == ADDRESS && t2->kind == ADDRESS)
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
            Operand t1 = temp->operand_3.result;
            Operand t2 = temp->operand_3.op1;
            Operand t3 = temp->operand_3.op2;
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
            Operand t1 = temp->operand_2.op1;
            Operand t2 = temp->operand_2.op2;
            fprintf(f, "t%d := &t%d", t1->u_int, t2->u_int);
        }
        else if (temp->kind == ADDRASS2)
        {
            Operand t1 = temp->operand_2.op1;
            Operand t2 = temp->operand_2.op2;
            fprintf(f, "t%d := *t%d", t1->u_int, t2->u_int);
        }
        else if (temp->kind == ADDRASS3)
        {
            Operand t1 = temp->operand_2.op1;
            Operand t2 = temp->operand_2.op2;
            fprintf(f, "*t%d := ", t1->u_int);
            if (t2->kind == CONSTANT)
                fprintf(f, "#%d", t2->u_int);
            else
                fprintf(f, "t%d", t2->u_int);
        }
        else if (temp->kind == GOTO)
        {
            fprintf(f, "GOTO label%d", temp->operand_1.op->u_int);
        }
        else if (temp->kind == IF)
        {
            Operand t1 = temp->operand_if.x;
            Operand re = temp->operand_if.relop;
            Operand t2 = temp->operand_if.y;
            Operand t3 = temp->operand_if.z;
            fprintf(f, "IF ");
            if (t1->kind == CONSTANT)
                fprintf(f, "#%d", t1->u_int);
            else if (t1->kind == ADDRESS)
                fprintf(f, "*t%d", t1->u_int);
            else
                fprintf(f, "t%d", t1->u_int);

            fprintf(f, " %s ", re->u_char);

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
            if (temp->operand_1.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->operand_1.op->u_int);
            else
                fprintf(f, "t%d", temp->operand_1.op->u_int);
        }
        else if (temp->kind == DEC)
        {
            fprintf(f, "DEC t%d %d", temp->operand_dec.op->u_int, temp->operand_dec.size);
        }
        else if (temp->kind == ARG)
        {
            fprintf(f, "ARG ");
            if (temp->operand_1.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->operand_1.op->u_int);
            else if (temp->operand_1.op->kind == ADDRESS)
                fprintf(f, "&t%d", temp->operand_1.op->u_int);
            else
                fprintf(f, "t%d", temp->operand_1.op->u_int);
        }
        else if (temp->kind == CALL)
        {
            Operand t1 = temp->operand_2.op1;
            Operand t2 = temp->operand_2.op2;
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
            if (temp->operand_1.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->operand_1.op->u_int);
            else
                fprintf(f, "t%d", temp->operand_1.op->u_int);
        }
        else if (temp->kind == READ)
        {
            fprintf(f, "READ ");
            if (temp->operand_1.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->operand_1.op->u_int);
            else
                fprintf(f, "t%d", temp->operand_1.op->u_int);
        }
        else if (temp->kind == WRITE)
        {
            fprintf(f, "WRITE ");
            if (temp->operand_1.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->operand_1.op->u_int);
            else
                fprintf(f, "t%d", temp->operand_1.op->u_int);
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
    P_Node child3;
    if (child2)
        child3 = child2->nextbro;
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
        place->kind = VARIABLE;
        insert_intercode(create_intercode(SUB, INTERCODE_3, place, create_operand(CONSTANT, 0, NULL), t1));
    }
    else if (strcmp(child2->Token, "ASSIGNOP") == 0)
    {
        // Exp → Exp ASSIGNOP Exp
        P_Node child_child = child->firstchild;
        Vari var = find_vari_table(child_child->Id_Type);
        Operand t1 = new_temp();
        translate_Exp(child3, t1);
        Operand t2 = create_operand(VARIABLE, var->variable, child_child->Id_Type);
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

void translate_Cond(P_Node now, Operand ltrue, Operand lfalse)
{
    P_Node child = now->firstchild;

    if (child->nextbro != NULL && strcmp(child->nextbro->Token, "RELOP") == 0)
    {
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        
        translate_Exp(child, t1);
        translate_Exp(child->nextbro->nextbro, t2);
        
        Operand op = create_operand(RELOP,OPERAND_NULL,child->nextbro->Id_Type);
        
        insert_intercode(create_intercode(IF, INTERCODE_IF, t1, op, t2, ltrue));
        insert_intercode(create_intercode(GOTO, INTERCODE_1, lfalse));
    }
    else if (strcmp(child->Token, "NOT") == 0)
    {
        translate_Cond(now->firstchild->nextbro, lfalse, ltrue);
    }
    else if (child->nextbro != NULL && strcmp(child->nextbro->Token, "AND") == 0)
    {
        Operand l1 = new_label();
        translate_Cond(child, l1, lfalse);
        insert_intercode(create_intercode(ILABEL, INTERCODE_1, l1));
        translate_Cond(child->nextbro->nextbro, ltrue, lfalse);
    }
    else if (child->nextbro != NULL && strcmp(child->nextbro->Token, "OR") == 0)
    {
        Operand l1 = new_label();
        translate_Cond(child, ltrue, l1);
        insert_intercode(create_intercode(ILABEL, INTERCODE_1, l1));
        translate_Cond(child->nextbro->nextbro, ltrue, lfalse);
    }
    else
    {
        Operand t1 = new_temp();
        translate_Exp(now, t1);
        Operand op = create_operand(RELOP, OPERAND_NULL, "!=");
        Operand t2 = create_operand(CONSTANT, 0, NULL);
        insert_intercode(create_intercode(IF, INTERCODE_IF, t1, op, t2, ltrue));
        insert_intercode(create_intercode(GOTO, INTERCODE_1, lfalse));
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
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        translate_VarDec(vardec, t1);
        translate_Exp(child2->nextbro, t2);

        insert_intercode(create_intercode(ASSIGN, INTERCODE_2, t1, t2));
    }
}

void translate_VarDec(P_Node now, Operand place)
{
    // VarDec -> ID | VarDec LB INT RB
    P_Node child = now->firstchild;
    place->kind = VARIABLE;
    place->u_char = child->Id_Type;
    if (child->nextbro == NULL)
    {
        // VarDec -> ID
        Vari var = find_vari_table(child->Id_Type);
        place->u_int = var->variable;
    }
    else
    {
        Vari var = find_vari_table(child->firstchild->Id_Type);
        place->u_int = var->variable;
        insert_intercode(create_intercode(DEC, INTERCODE_DEC, place, get_size(var->field->type)));
    }
}

void translate_Args(P_Node now, InterCode here)
{
    // Args → Exp COMMA Args| Exp
    P_Node exp = now->firstchild;
    Operand t1 = new_temp();
    translate_Exp(exp, t1);
    if (t1->kind != CONSTANT && t1->kind != ADDRESS)
    {
        Vari var = find_vari_table(t1->u_char);
        if (var == NULL)
            t1->kind = VARIABLE;
        else if (var->field->type->type != BASIC)
            t1->kind = ADDRESS;
        else
            t1->kind = VARIABLE;
    }
    InterCode code1_in = create_intercode(ARG, INTERCODE_1, t1);
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
    P_Node comma = exp->nextbro;
    if (comma)
        translate_Args(comma->nextbro, code1_in);
    insert_intercode(code1_in);
}

Operand new_temp()
{
    return create_operand(UNKNOWN, temp_num++, "Temp");
}

Operand new_label()
{
    return create_operand(LABEL, label_num++, NULL);
}

void insert_intercode(InterCode this)
{
    this->prev = NULL;
    this->next = NULL;
    if (in_head == NULL)
        in_head = in_now = this;
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
        return get_size(return_type->array.elem);
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
