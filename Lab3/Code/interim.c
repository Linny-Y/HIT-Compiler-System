#include "interim.h"

int lab_num = 1;          //用于LABEL的命名
int interim_is_error = 0; //程序无错=0，有错=1
InterCode in_head = NULL; //记录双向链表的头部
InterCode in_now = NULL;  //记录现在的双向链表
extern int temp_num;
//对于符号表中的符号，在加入table时就赋予其数字

void translate_print(FILE *f)
{
    // printf("a translate_print\n");
    InterCode temp = in_head;
    // InterCode temp=in_now;
    // in_head->before=NULL;
    int judge_is_null = 0;
    int a = 0;
    // add,sub,mul,div没考虑null

    while (temp != NULL)
    {
        if (temp->kind == ILABEL)
        {
            // printf("a translate_print ILABEL %d\n",temp->u.ulabel.op->u_int);
            fprintf(f, "LABEL label");
            fprintf(f, "%d ", temp->u.ulabel.op->u_int);
            fprintf(f, ":");
        }
        else if (temp->kind == IFUNCTION)
        {
            // printf("a translate_print IFUNCTION\n");
            fprintf(f, "FUNCTION ");
            fprintf(f, "%s ", temp->u.ulabel.op->u_char);
            fprintf(f, ":");
        }
        else if (temp->kind == ASSIGN)
        {
            // printf("a translate_print ASSIGN\n");
            Operand t1 = temp->u.uassign.op1;
            Operand t2 = temp->u.uassign.op2;
            if (t1 == NULL || t2 == NULL)
            {
                judge_is_null = 1;
                // printf("a 1\n");
            }
            else if (t1->kind == ADDRESS && t2->kind == ADDRESS)
            {
                // printf("a 1.1\n");
                fprintf(f, "*t%d", t1->u_int);
                fprintf(f, " := ");
                fprintf(f, "*t%d", t2->u_int);
            }
            else if (t2->kind == ADDRESS)
            {
                // printf("a 1.3\n");
                fprintf(f, "t%d", t1->u_int);
                fprintf(f, " := ");
                fprintf(f, "*t%d", t2->u_int);
            }
            else if (t1->kind == ADDRESS)
            {
                // printf("a 1.2\n");
                fprintf(f, "&t%d", t1->u_int);
                fprintf(f, " := ");
                if (t2->kind == CONSTANT)
                    fprintf(f, "#%d", t2->u_int);
                else
                    fprintf(f, "t%d", t2->u_int);
            }
            else
            {
                // printf("a 1.4\n");
                fprintf(f, "t%d", t1->u_int);
                fprintf(f, " := ");
                if (t2->kind == CONSTANT)
                    fprintf(f, "#%d", t2->u_int);
                else
                    fprintf(f, "t%d", t2->u_int);
            }
        }
        else if (temp->kind == ADD)
        {
            // printf("a translate_print  ADD\n");
            Operand t1 = temp->u.ubinop.result;
            Operand t2 = temp->u.ubinop.op1;
            Operand t3 = temp->u.ubinop.op2;
            if (t1 == NULL)
                ;
            else
            {
                fprintf(f, "t%d", t1->u_int);
                fprintf(f, " := ");
                if (t2->kind == CONSTANT)
                    fprintf(f, "#%d", t2->u_int);
                else
                    fprintf(f, "t%d", t2->u_int);
                fprintf(f, " + ");
                if (t3->kind == CONSTANT)
                    fprintf(f, "#%d", t3->u_int);
                else
                    fprintf(f, "t%d", t3->u_int);
            }
        }
        else if (temp->kind == SUB)
        {
            // printf("a translate_print SUB\n");
            Operand t1 = temp->u.ubinop.result;
            Operand t2 = temp->u.ubinop.op1;
            Operand t3 = temp->u.ubinop.op2;
            if (t1 == NULL)
                ;
            else
            {
                fprintf(f, "t%d", t1->u_int);
                fprintf(f, " := ");
                if (t2->kind == CONSTANT)
                    fprintf(f, "#%d", t2->u_int);
                else
                    fprintf(f, "t%d", t2->u_int);
                fprintf(f, " - ");
                if (t3->kind == CONSTANT)
                    fprintf(f, "#%d", t3->u_int);
                else
                    fprintf(f, "t%d", t3->u_int);
            }
        }
        else if (temp->kind == MUL)
        {
            // printf("a translate_print MUL\n");
            Operand t1 = temp->u.ubinop.result;
            Operand t2 = temp->u.ubinop.op1;
            Operand t3 = temp->u.ubinop.op2;
            if (t1 == NULL)
                ;
            else
            {
                fprintf(f, "t%d", t1->u_int);
                fprintf(f, " := ");
                if (t2->kind == CONSTANT)
                    fprintf(f, "#%d", t2->u_int);
                else
                    fprintf(f, "t%d", t2->u_int);
                fprintf(f, " * ");
                if (t3->kind == CONSTANT)
                    fprintf(f, "#%d", t3->u_int);
                else
                    fprintf(f, "t%d", t3->u_int);
            }
        }
        else if (temp->kind == DIV)
        {
            // printf("a translate_print DIV\n");
            Operand t1 = temp->u.ubinop.result;
            Operand t2 = temp->u.ubinop.op1;
            Operand t3 = temp->u.ubinop.op2;
            if (t1 == NULL)
                ;
            else
            {
                fprintf(f, "t%d", t1->u_int);
                fprintf(f, " := ");
                if (t2->kind == CONSTANT)
                    fprintf(f, "#%d", t2->u_int);
                else
                    fprintf(f, "t%d", t2->u_int);
                fprintf(f, " / ");
                if (t3->kind == CONSTANT)
                    fprintf(f, "#%d", t3->u_int);
                else
                    fprintf(f, "t%d", t3->u_int);
            }
        }
        else if (temp->kind == ADDRASS2)
        {
            // printf("a translate_print ADDRASS2\n");
            Operand t1 = temp->u.uassign.op1;
            Operand t2 = temp->u.uassign.op2;
            fprintf(f, "t%d", t1->u_int);
            fprintf(f, " := ");
            fprintf(f, "*t%d", t2->u_int);
        }
        else if (temp->kind == ADDRASS3)
        {
            // printf("a translate_print ADDRASS3\n");
            Operand t1 = temp->u.uassign.op1;
            Operand t2 = temp->u.uassign.op2;
            fprintf(f, "*t%d", t1->u_int);
            fprintf(f, " := ");
            if (t2->kind == CONSTANT)
                fprintf(f, "#%d", t2->u_int);
            else
                fprintf(f, "t%d", t2->u_int);
        }
        else if (temp->kind == ADDRASS1)
        {
            // printf("a translate_print ADDRASS2\n");
            Operand t1 = temp->u.uassign.op1;
            Operand t2 = temp->u.uassign.op2;
            fprintf(f, "t%d", t1->u_int);
            fprintf(f, " := ");
            fprintf(f, "&t%d", t2->u_int);
        }
        else if (temp->kind == GOTO)
        {
            // printf("a translate_print GOTO\n");
            fprintf(f, "GOTO label");
            fprintf(f, "%d", temp->u.ulabel.op->u_int);
        }
        else if (temp->kind == IF)
        {
            // printf("a translate_print IF\n");
            Operand t1 = temp->u.uif.x;
            Operand re = temp->u.uif.relop;
            Operand t2 = temp->u.uif.y;
            Operand t3 = temp->u.uif.z;
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
            fprintf(f, " GOTO label");
            fprintf(f, "%d", t3->u_int);
        }
        else if (temp->kind == RETURN)
        {
            // printf("a translate_print RETURN\n");
            fprintf(f, "RETURN ");
            if (temp->u.ulabel.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->u.ulabel.op->u_int);
            else
                fprintf(f, "t%d", temp->u.ulabel.op->u_int);
        }
        else if (temp->kind == DEC)
        {
            // printf("a translate_print DEC\n");
            fprintf(f, "DEC ");
            fprintf(f, "t%d ", temp->u.udec.op->u_int);
            fprintf(f, "%d", temp->u.udec.size);
        }
        else if (temp->kind == ARG)
        {
            // printf("a translate_print ARG\n");
            fprintf(f, "ARG ");
            if (temp->u.ulabel.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->u.ulabel.op->u_int);
            else if (temp->u.ulabel.op->kind == ADDRESS)
                fprintf(f, "&t%d", temp->u.ulabel.op->u_int);
            else if (temp->u.ulabel.op->kind == WADDRESS)
                fprintf(f, "*t%d", temp->u.ulabel.op->u_int);
            else
                fprintf(f, "t%d", temp->u.ulabel.op->u_int);
        }
        else if (temp->kind == CALL)
        {
            // printf("a translate_print CALL\n");
            Operand t1 = temp->u.uassign.op1;
            Operand t2 = temp->u.uassign.op2;
            if (t1 != NULL)
                fprintf(f, "t%d := ", t1->u_int);
            else
            {
                Operand t0 = new_temp();
                fprintf(f, "t%d := ", t0->u_int);
            }
            fprintf(f, "CALL ");
            ////printf("a %s\n",t2->kind);
            fprintf(f, "%s", t2->u_char);
        }
        else if (temp->kind == PARAM)
        {
            // printf("a translate_print PARAM\n");
            fprintf(f, "PARAM ");
            if (temp->u.ulabel.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->u.ulabel.op->u_int);
            else
                fprintf(f, "t%d", temp->u.ulabel.op->u_int);
        }
        else if (temp->kind == READ)
        {
            // printf("a translate_print READ\n");
            fprintf(f, "READ ");
            if (temp->u.ulabel.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->u.ulabel.op->u_int);
            else
                fprintf(f, "t%d", temp->u.ulabel.op->u_int);
        }
        else if (temp->kind == WRITE)
        {
            // printf("a translate_print WRITE\n");
            fprintf(f, "WRITE ");
            if (temp->u.ulabel.op->kind == CONSTANT)
                fprintf(f, "#%d", temp->u.ulabel.op->u_int);
            else
                fprintf(f, "t%d", temp->u.ulabel.op->u_int);
        }
        if (judge_is_null == 0)
            fprintf(f, "\n");
        else
            judge_is_null = 0;
        // fprintf(f,"\n");
        ////printf("a 2\n");
        temp = temp->next;
        // temp=temp->before;
    }
}

void translate_Program(P_Node now, FILE *F)
{
    P_Node extdeflist = now->firstchild;
    while (extdeflist->firstchild != NULL)
    {
        P_Node extdef = extdeflist->firstchild;
        translate_ExtDef(extdef);
        extdeflist = extdef->nextbro;
    }
    translate_print(F);
}

void translate_ExtDef(P_Node now)
{
    /*
    ExtDef → Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt
    */

    P_Node after_specifier = now->firstchild->nextbro;
    if (strcmp(after_specifier->Token, "FunDec") == 0)
    {
        translate_FunDec(after_specifier);
        translate_CompSt(after_specifier->nextbro);
    }
    else if (strcmp(now->firstchild->Token, "Specifier") == 0)
    {
        translate_Specifier(now->firstchild);
    }
}

void translate_FunDec(P_Node now)
{
    // FunDec → ID LP VarList RP| ID LP RP
    //翻译id，生成语句
    Operand func_op = (Operand)malloc(sizeof(struct Operand_));
    func_op->kind = FUNCTION;
    func_op->u_char = now->firstchild->Id_Type;
    InterCode func_in = (InterCode)malloc(sizeof(struct InterCode_));
    func_in->kind = IFUNCTION;
    func_in->u.ulabel.op = func_op;
    add_to_intercode(func_in);
    //翻译varlist，参数列表
    if (now->firstchild->nextbro->nextbro->nextbro == NULL)
        return;
    Function find_func = find_function_table(now->firstchild->Id_Type);
    FieldList func_field = find_func->field;
    while (func_field != NULL)
    {
        // 1.每一个参数生成一个对应的operand，根据结构体、非结构体设置kind
        Operand field_op = (Operand)malloc(sizeof(struct Operand_));
        if (func_field->type->type == STRUCT || func_field->type->type == ARRAY)
            field_op->kind = ADDRESS;
        else
            field_op->kind = VARIABLE;
        //记录用户给的名称，以及符号表中赋予的Num
        Vari find_it = find_vari_table(func_field->name);
        field_op->u_int = find_it->variable;
        field_op->u_char = func_field->name;
        // 2.每个参数生成一条中间代码
        InterCode field_in = (InterCode)malloc(sizeof(struct InterCode_));
        field_in->kind = PARAM;
        field_in->u.ulabel.op = field_op;
        add_to_intercode(field_in);
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
    // DefList → Def DefList| 
    P_Node newdef = now->firstchild;
    while (newdef != NULL)
    {
        translate_Def(newdef);
        newdef = newdef->nextbro->firstchild;
    }
}

void translate_StmtList(P_Node now)
{
    // StmtList → Stmt StmtList| 
    P_Node child = now->firstchild;
    while (child != NULL)
    {
        translate_Stmt(child);
        child = child->nextbro->firstchild;
    }
}

void translate_Def(P_Node now)
{
    // Def → Specifier DecList SEMI
    P_Node declist = now->firstchild->nextbro;
    translate_DecList(declist);
}

void translate_Stmt(P_Node now)
{
    /*
    Stmt → Exp SEMI
    | CompSt
    | RETURN Exp SEMI
    | IF LP Exp RP Stmt
    | IF LP Exp RP Stmt ELSE Stmt
    | WHILE LP Exp RP Stmt
    */
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
        Operand t0 = NULL;
        if (t1->kind == ADDRESS)
        {
            t0 = new_temp();
            InterCode code0_in = (InterCode)malloc(sizeof(struct InterCode_));
            code0_in->kind = ADDRASS2;
            code0_in->u.uassign.op1 = t0;
            code0_in->u.uassign.op2 = t1;
            add_to_intercode(code0_in);
        }

        InterCode return_in = (InterCode)malloc(sizeof(struct InterCode_));
        return_in->kind = RETURN;
        if (t0 == NULL)
            return_in->u.ulabel.op = t1;
        else
            return_in->u.ulabel.op = t0;
        add_to_intercode(return_in);
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
            translate_Cond(child->nextbro->nextbro, l1, l2);
            InterCode l1_in = (InterCode)malloc(sizeof(struct InterCode_));
            l1_in->kind = ILABEL;
            l1_in->u.ulabel.op = l1;
            add_to_intercode(l1_in);
            translate_Stmt(child->nextbro->nextbro->nextbro->nextbro);
            InterCode l2_in = (InterCode)malloc(sizeof(struct InterCode_));
            l2_in->kind = ILABEL;
            l2_in->u.ulabel.op = l2;
            add_to_intercode(l2_in);
            return;
        }
        else
        {
            // IF LP Exp RP Stmt ELSE Stmt
            Operand l1 = new_label();
            Operand l2 = new_label();
            Operand l3 = new_label();
            // code1
            translate_Cond(child->nextbro->nextbro, l1, l2);
            // LABEL L1
            InterCode l1_in = (InterCode)malloc(sizeof(struct InterCode_));
            l1_in->kind = ILABEL;
            l1_in->u.ulabel.op = l1;
            add_to_intercode(l1_in);
            // code2
            translate_Stmt(child->nextbro->nextbro->nextbro->nextbro);
            // GOTO label3
            InterCode l2_in = (InterCode)malloc(sizeof(struct InterCode_));
            l2_in->kind = GOTO;
            l2_in->u.ulabel.op = l3;
            add_to_intercode(l2_in);
            // LABEL l2
            InterCode l3_in = (InterCode)malloc(sizeof(struct InterCode_));
            l3_in->kind = ILABEL;
            l3_in->u.ulabel.op = l2;
            add_to_intercode(l3_in);
            // code3
            translate_Stmt(child->nextbro->nextbro->nextbro->nextbro->nextbro->nextbro);
            // LABEL l3
            InterCode l4_in = (InterCode)malloc(sizeof(struct InterCode_));
            l4_in->kind = ILABEL;
            l4_in->u.ulabel.op = l3;
            add_to_intercode(l4_in);
            return;
        }
    }
    else if (strcmp(child->Token, "WHILE") == 0)
    {
        // Stmt →WHILE LP Exp RP Stmt
        Operand l1 = new_label();
        Operand l2 = new_label();
        Operand l3 = new_label();
        // LABEL L1
        InterCode l1_in = (InterCode)malloc(sizeof(struct InterCode_));
        l1_in->kind = ILABEL;
        l1_in->u.ulabel.op = l1;
        add_to_intercode(l1_in);
        // code1
        translate_Cond(child->nextbro->nextbro, l2, l3);
        // LABEL l2
        InterCode l3_in = (InterCode)malloc(sizeof(struct InterCode_));
        l3_in->kind = ILABEL;
        l3_in->u.ulabel.op = l2;
        add_to_intercode(l3_in);
        // code2
        translate_Stmt(child->nextbro->nextbro->nextbro->nextbro);
        // GOTO label1
        InterCode l2_in = (InterCode)malloc(sizeof(struct InterCode_));
        l2_in->kind = GOTO;
        l2_in->u.ulabel.op = l1;
        add_to_intercode(l2_in);
        // LABEL l3
        InterCode l4_in = (InterCode)malloc(sizeof(struct InterCode_));
        l4_in->kind = ILABEL;
        l4_in->u.ulabel.op = l3;
        add_to_intercode(l4_in);
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
    P_Node fir_bro = child->nextbro;
    if (fir_bro == NULL)
    {
        if (strcmp(child->Token, "INT") == 0)
        {
            // Exp → INT
            // printf("a translate_Exp INT\n");
            int value = child->intVal;
            if (place == NULL)
                return;
            place->kind = CONSTANT;
            place->u_int = value;
            return;
        }
        //没有exp->float
        else if (strcmp(child->Token, "ID") == 0)
        {
            // Exp → ID
            //这里也可能出现结构体以及数组，均返回address
            //如op.x=1因此要考虑
            Vari find_id = find_vari_table(child->Id_Type);
            char *name = child->Id_Type;
            if (place == NULL)
                return;
            if (find_id->is_from_func == 0 && find_id->field->type->type != BASIC)
                place->kind = ADDRESS;
            else
                place->kind = VARIABLE;
            place->u_char = name;
            place->u_int = find_id->variable;
            place->type = find_id->field->type;
            return;
        }
    }
    P_Node sec_bro = fir_bro->nextbro;
    if (sec_bro == NULL)
    {
        if (strcmp(child->Token, "MINUS") == 0)
        {
            // Exp → MINUS Exp
            if (place == NULL)
                return;
            // printf("a translate_Exp MINUS\n");
            Operand t1 = new_temp();
            translate_Exp(child->nextbro, t1);
            InterCode minus_in = (InterCode)malloc(sizeof(struct InterCode_));
            minus_in->kind = SUB;
            minus_in->u.ubinop.result = place;
            minus_in->u.ubinop.op2 = t1;
            Operand t2 = (Operand)malloc(sizeof(struct Operand_));
            t2->kind = CONSTANT;
            t2->u_int = 0;
            minus_in->u.ubinop.op1 = t2;
            if (place->kind == FROM_ARG)
                place->kind = VARIABLE;
            // printf("a ater_print\n");
            add_to_intercode(minus_in);
            // printf("a MINUSEXPout\n");
            return;
        }
        else if (strcmp(child->Token, "NOT") == 0)
        {
            // Exp →NOT Exp
            Operand l1 = new_label();
            Operand l2 = new_label();
            // code0
            InterCode code0_in = (InterCode)malloc(sizeof(struct InterCode_));
            code0_in->kind = ASSIGN;
            code0_in->u.uassign.op1 = place;
            Operand t1 = (Operand)malloc(sizeof(struct Operand_));
            t1->kind = CONSTANT;
            t1->u_int = 0;
            code0_in->u.uassign.op2 = t1;
            add_to_intercode(code0_in);
            // code1
            translate_Cond(now, l1, l2);
            // code2-1
            InterCode code2_in = (InterCode)malloc(sizeof(struct InterCode_));
            code2_in->kind = ILABEL;
            code2_in->u.ulabel.op = l1;
            add_to_intercode(code2_in);
            // code2-2
            InterCode code22_in = (InterCode)malloc(sizeof(struct InterCode_));
            code22_in->kind = ASSIGN;
            code22_in->u.uassign.op1 = place;
            Operand t2 = (Operand)malloc(sizeof(struct Operand_));
            t2->kind = CONSTANT;
            t2->u_int = 1;
            code22_in->u.uassign.op2 = t2;
            add_to_intercode(code22_in);
            // LABEL l2
            InterCode label_in = (InterCode)malloc(sizeof(struct InterCode_));
            label_in->kind = ILABEL;
            label_in->u.ulabel.op = l2;
            add_to_intercode(label_in);
            return;
        }
    }
    if (strcmp(fir_bro->Token, "ASSIGNOP") == 0)
    {
        // Exp → Exp ASSIGNOP Exp
        if (strcmp(child->firstchild->Token, "ID") == 0)
        {
            Operand t1 = new_temp();
            // code1
            translate_Exp(fir_bro->nextbro, t1);
            // code21
            InterCode code21_in = (InterCode)malloc(sizeof(struct InterCode_));
            code21_in->kind = ASSIGN;
            code21_in->u.uassign.op2 = t1;
            Operand t2 = (Operand)malloc(sizeof(struct Operand_));
            Vari find_it = find_vari_table(child->firstchild->Id_Type);
            t2->kind = VARIABLE;
            t2->u_char = child->firstchild->Id_Type;
            t2->u_int = find_it->variable;
            code21_in->u.uassign.op1 = t2;
            add_to_intercode(code21_in);
            // code22
            // printf("a 2\n");
            InterCode code22_in = (InterCode)malloc(sizeof(struct InterCode_));
            code22_in->kind = ASSIGN;
            code22_in->u.uassign.op1 = place;
            code22_in->u.uassign.op2 = t2;
            add_to_intercode(code22_in);
            return;
        }
        else
        {
            Operand t1 = new_temp();
            // code1
            translate_Exp(fir_bro->nextbro, t1);
            Operand t0 = NULL;
            if (t1->kind == ADDRESS)
            {
                //生成t0=*t1语句
                t0 = new_temp();
                InterCode in_1 = (InterCode)malloc(sizeof(struct InterCode_));
                in_1->kind = ADDRASS2;
                in_1->u.uassign.op1 = t0;
                in_1->u.uassign.op2 = t1;
                add_to_intercode(in_1);
            }
            // code21
            InterCode code21_in = (InterCode)malloc(sizeof(struct InterCode_));
            code21_in->kind = ADDRASS3;
            if (t0 == NULL)
                code21_in->u.uassign.op2 = t1;
            else
                code21_in->u.uassign.op2 = t0;
            Operand t2 = new_temp();
            translate_Exp(child, t2);
            code21_in->u.uassign.op1 = t2;
            add_to_intercode(code21_in);
            // code22
            // printf("a 2\n");
            InterCode code22_in = (InterCode)malloc(sizeof(struct InterCode_));
            code22_in->kind = ASSIGN;
            code22_in->u.uassign.op1 = place;
            code22_in->u.uassign.op2 = t2;
            add_to_intercode(code22_in);
            return;
        }
    }
    if (strcmp(fir_bro->Token, "AND") == 0 || strcmp(fir_bro->Token, "OR") == 0 || strcmp(fir_bro->Token, "RELOP") == 0)
    {
        // Exp → Exp AND\OR\RELOP Exp
        Operand l1 = new_label();
        Operand l2 = new_label();
        // code0
        InterCode code0_in = (InterCode)malloc(sizeof(struct InterCode_));
        code0_in->kind = ASSIGN;
        code0_in->u.uassign.op1 = place;
        Operand t1 = (Operand)malloc(sizeof(struct Operand_));
        t1->kind = CONSTANT;
        t1->u_int = 0;
        code0_in->u.uassign.op2 = t1;
        add_to_intercode(code0_in);
        // code1
        translate_Cond(now, l1, l2);
        // code2-1
        InterCode code2_in = (InterCode)malloc(sizeof(struct InterCode_));
        code2_in->kind = ILABEL;
        code2_in->u.ulabel.op = l1;
        add_to_intercode(code2_in);
        // code2-2
        InterCode code22_in = (InterCode)malloc(sizeof(struct InterCode_));
        code22_in->kind = ASSIGN;
        code22_in->u.uassign.op1 = place;
        Operand t2 = (Operand)malloc(sizeof(struct Operand_));
        t2->kind = CONSTANT;
        t2->u_int = 1;
        code22_in->u.uassign.op2 = t2;
        add_to_intercode(code22_in);
        // LABEL l2
        InterCode label_in = (InterCode)malloc(sizeof(struct InterCode_));
        label_in->kind = ILABEL;
        label_in->u.ulabel.op = l2;
        add_to_intercode(label_in);
        // printf("a ANDout\n");
        return;
    }
    if (strcmp(fir_bro->Token, "PLUS") == 0)
    {
        // Exp → Exp PLUS Exp
        // printf("a translate_Exp PLUS\n");
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        translate_Exp(child, t1);
        translate_Exp(fir_bro->nextbro, t2);
        Operand t3 = NULL;
        Operand t4 = NULL;
        if (t1->kind == CONSTANT && t2->kind == CONSTANT)
            place->kind = VARIABLE;
        if (t1->kind == ADDRESS)
        {
            t3 = new_temp();
            InterCode in1 = (InterCode)malloc(sizeof(struct InterCode_));
            in1->kind = ADDRASS2;
            in1->u.uassign.op1 = t3;
            in1->u.uassign.op2 = t1;
            add_to_intercode(in1);
        }
        if (t2->kind == ADDRESS)
        {
            t4 = new_temp();
            InterCode in1 = (InterCode)malloc(sizeof(struct InterCode_));
            in1->kind = ADDRASS2;
            in1->u.uassign.op1 = t4;
            in1->u.uassign.op2 = t2;
            add_to_intercode(in1);
        }
        InterCode code0_in = (InterCode)malloc(sizeof(struct InterCode_));
        code0_in->kind = ADD;
        code0_in->u.ubinop.result = place;
        if (t3 == NULL)
            code0_in->u.ubinop.op1 = t1;
        else
            code0_in->u.ubinop.op1 = t3;
        if (t4 == NULL)
            code0_in->u.ubinop.op2 = t2;
        else
            code0_in->u.ubinop.op2 = t4;
        add_to_intercode(code0_in);
        // printf("a PLUSout\n");
        return;
    }
    if (strcmp(fir_bro->Token, "MINUS") == 0)
    {
        // Exp → Exp MINUS Exp
        // printf("a translate_Exp MINUS\n");
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        translate_Exp(child, t1);
        translate_Exp(fir_bro->nextbro, t2);
        Operand t3 = NULL;
        Operand t4 = NULL;
        if (t1->kind == CONSTANT && t2->kind == CONSTANT)
            place->kind = VARIABLE;
        if (t1->kind == ADDRESS)
        {
            t3 = new_temp();
            InterCode in1 = (InterCode)malloc(sizeof(struct InterCode_));
            in1->kind = ADDRASS2;
            in1->u.uassign.op1 = t3;
            in1->u.uassign.op2 = t1;
            add_to_intercode(in1);
        }
        if (t2->kind == ADDRESS)
        {
            t4 = new_temp();
            InterCode in1 = (InterCode)malloc(sizeof(struct InterCode_));
            in1->kind = ADDRASS2;
            in1->u.uassign.op1 = t4;
            in1->u.uassign.op2 = t2;
            add_to_intercode(in1);
        }
        InterCode code0_in = (InterCode)malloc(sizeof(struct InterCode_));
        code0_in->kind = SUB;
        code0_in->u.ubinop.result = place;
        if (t3 == NULL)
            code0_in->u.ubinop.op1 = t1;
        else
            code0_in->u.ubinop.op1 = t3;
        if (t4 == NULL)
            code0_in->u.ubinop.op2 = t2;
        else
            code0_in->u.ubinop.op2 = t4;
        add_to_intercode(code0_in);
        // printf("a MINUSout\n");
        return;
    }
    if (strcmp(fir_bro->Token, "STAR") == 0)
    {
        // Exp → Exp STAR Exp
        // printf("a translate_Exp STAR\n");
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        translate_Exp(child, t1);
        translate_Exp(fir_bro->nextbro, t2);
        if (t1->kind == CONSTANT && t2->kind == CONSTANT)
            place->kind = VARIABLE;
        Operand t3 = NULL;
        Operand t4 = NULL;
        if (t1->kind == ADDRESS)
        {
            t3 = new_temp();
            InterCode in1 = (InterCode)malloc(sizeof(struct InterCode_));
            in1->kind = ADDRASS2;
            in1->u.uassign.op1 = t3;
            in1->u.uassign.op2 = t1;
            add_to_intercode(in1);
        }
        if (t2->kind == ADDRESS)
        {
            t4 = new_temp();
            InterCode in1 = (InterCode)malloc(sizeof(struct InterCode_));
            in1->kind = ADDRASS2;
            in1->u.uassign.op1 = t4;
            in1->u.uassign.op2 = t2;
            add_to_intercode(in1);
        }
        InterCode code0_in = (InterCode)malloc(sizeof(struct InterCode_));
        code0_in->kind = MUL;
        code0_in->u.ubinop.result = place;
        if (t3 == NULL)
            code0_in->u.ubinop.op1 = t1;
        else
            code0_in->u.ubinop.op1 = t3;
        if (t4 == NULL)
            code0_in->u.ubinop.op2 = t2;
        else
            code0_in->u.ubinop.op2 = t4;
        add_to_intercode(code0_in);
        return;
    }
    if (strcmp(fir_bro->Token, "DIV") == 0)
    {
        // Exp → Exp DIV Exp
        // printf("a translate_Exp DIV\n");
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        translate_Exp(child, t1);
        translate_Exp(fir_bro->nextbro, t2);
        // printf("a OUTDIV\n");
        Operand t3 = NULL;
        Operand t4 = NULL;
        if (t1->kind == CONSTANT && t2->kind == CONSTANT)
            place->kind = VARIABLE;
        if (t1->kind == ADDRESS)
        {
            t3 = new_temp();
            InterCode in1 = (InterCode)malloc(sizeof(struct InterCode_));
            in1->kind = ADDRASS2;
            in1->u.uassign.op1 = t3;
            in1->u.uassign.op2 = t1;
            add_to_intercode(in1);
        }
        // printf("a OUTDIV2\n");
        if (t2->kind == ADDRESS)
        {
            t4 = new_temp();
            InterCode in1 = (InterCode)malloc(sizeof(struct InterCode_));
            in1->kind = ADDRASS2;
            in1->u.uassign.op1 = t4;
            in1->u.uassign.op2 = t2;
            add_to_intercode(in1);
        }
        // printf("a OUTDIV3\n");
        InterCode code0_in = (InterCode)malloc(sizeof(struct InterCode_));
        code0_in->kind = DIV;
        code0_in->u.ubinop.result = place;
        if (t3 == NULL)
            code0_in->u.ubinop.op1 = t1;
        else
            code0_in->u.ubinop.op1 = t3;
        if (t4 == NULL)
            code0_in->u.ubinop.op2 = t2;
        else
            code0_in->u.ubinop.op2 = t4;
        add_to_intercode(code0_in);
        return;
    }
    if (strcmp(fir_bro->Token, "DOT") == 0)
    {
        // Exp → Exp DOT ID,结构体
        // 1.获取结构体的首地址 t2=t1+#4, t1\v1->address
        // printf("a translate_Exp DOT\n");
        Operand t1 = new_temp();
        translate_Exp(child, t1);
        if (t1->kind == VARIABLE)
        {
            // 0.有address和variable的区别
            //需要增加一个&d的操作
            // printf("a Exp DOT ID-VARIABLE\n");
            Operand t0 = new_temp();
            t0->kind = ADDRESS;
            InterCode in = (InterCode)malloc(sizeof(struct InterCode_));
            in->kind = ADDRASS1;
            in->u.uassign.op1 = t0;
            in->u.uassign.op2 = t1;
            add_to_intercode(in);
            // 2.获取当前domain的偏移量
            int offset = get_offset(t1->type, fir_bro->nextbro);
            if (offset == -1)
                return;
            Operand t3 = (Operand)malloc(sizeof(struct Operand_));
            t3->kind = CONSTANT;
            t3->u_int = offset;
            // 3.计算获得现在的地址值，打印中间代码
            if (place == NULL)
                return;
            if (place->kind == FROM_ARG)
                place->kind = WADDRESS;
            else
                place->kind = ADDRESS;
            Vari F = find_vari_table(fir_bro->nextbro->Id_Type);
            place->type = F->field->type;
            // Operand t2=new_temp();
            // t2->kind=ADDRESS;
            InterCode code0_in = (InterCode)malloc(sizeof(struct InterCode_));
            code0_in->kind = ADD;
            code0_in->u.ubinop.result = place;
            code0_in->u.ubinop.op1 = t0;
            code0_in->u.ubinop.op2 = t3;
            add_to_intercode(code0_in);
        }
        else
        {
            // 2.获取当前domain的偏移量
            // printf("a Exp DOT ID-address\n");
            int offset = get_offset(t1->type, fir_bro->nextbro);
            if (offset == -1)
                return;
            Operand t3 = (Operand)malloc(sizeof(struct Operand_));
            t3->kind = CONSTANT;
            t3->u_int = offset;
            // 3.计算获得现在的地址值，打印中间代码
            //  Operand t2=new_temp();
            //  t2->kind=ADDRESS;
            if (place == NULL)
                return;
            if (place->kind == FROM_ARG)
                place->kind = WADDRESS;
            else
                place->kind = ADDRESS;
            // printf("a %s\n",fir_bro->nextbro->Id_Type);
            Vari F = find_vari_table(fir_bro->nextbro->Id_Type);
            place->type = F->field->type;

            InterCode code0_in = (InterCode)malloc(sizeof(struct InterCode_));
            code0_in->kind = ADD;
            code0_in->u.ubinop.result = place;
            code0_in->u.ubinop.op1 = t1;
            code0_in->u.ubinop.op2 = t3;
            add_to_intercode(code0_in);
        }
        return;
    }
    if (strcmp(fir_bro->Token, "LB") == 0)
    {
        // Exp → Exp LB Exp RB，数组
        // 1.获取数组的首地址
        // printf("a translate_Exp LB\n");
        Operand t1 = new_temp();
        translate_Exp(child, t1);
        if (t1->kind == VARIABLE)
        {
            // 0.有address和variable的区别
            //需要增加一个&d的操作
            // printf("a translate_Exp LB VARIABLE\n");
            Operand t0 = new_temp();
            t0->kind = ADDRESS;
            InterCode in = (InterCode)malloc(sizeof(struct InterCode_));
            in->kind = ADDRASS1;
            in->u.uassign.op1 = t0;
            in->u.uassign.op2 = t1;
            add_to_intercode(in);
            // 2.获取数组单个单位的长度offset
            int offset = get_offset(t1->type, NULL);
            // printf("a %s %d\n",child->firstchild->Id_Type,offset);
            if (offset == -1)
                return;
            // printf("a offset:%d\n",offset);
            // 3.生成t2=offset*i
            Operand i = new_temp();
            translate_Exp(fir_bro->nextbro, i);
            Operand t2 = new_temp();
            Operand o_offset = (Operand)malloc(sizeof(struct Operand_));
            o_offset->kind = CONSTANT;
            o_offset->u_int = offset;
            InterCode in2 = (InterCode)malloc(sizeof(struct InterCode_));
            in2->kind = MUL;
            in2->u.ubinop.result = t2;
            in2->u.ubinop.op1 = i;
            in2->u.ubinop.op2 = o_offset;
            add_to_intercode(in2);
            // 4.生成中间代码t3=t1+t2
            //  Operand t3=new_temp();
            //  t3->kind=ADDRESS;
            if (place == NULL)
                return;
            if (place->kind == FROM_ARG)
                place->kind = WADDRESS;
            else
                place->kind = ADDRESS;
            place->type = t1->type->array.elem;
            InterCode code0_in = (InterCode)malloc(sizeof(struct InterCode_));
            code0_in->kind = ADD;
            code0_in->u.ubinop.result = place;
            code0_in->u.ubinop.op1 = t0;
            code0_in->u.ubinop.op2 = t2;
            add_to_intercode(code0_in);
        }
        else
        {
            // 2.获取数组单个单位的长度offset,生成t2=offset*i
            // printf("a translate_Exp LB NON-VARIABLE\n");
            int offset = get_offset(t1->type, NULL);
            if (offset == -1)
                return;
            Operand i = new_temp();
            translate_Exp(fir_bro->nextbro, i);
            Operand t2 = new_temp();
            Operand o_offset = (Operand)malloc(sizeof(struct Operand_));
            o_offset->kind = CONSTANT;
            o_offset->u_int = offset;
            InterCode in2 = (InterCode)malloc(sizeof(struct InterCode_));
            in2->kind = MUL;
            in2->u.ubinop.result = t2;
            in2->u.ubinop.op1 = i;
            in2->u.ubinop.op2 = o_offset;
            add_to_intercode(in2);
            // 4.生成中间代码t3=t1+t2
            // Operand t3=new_temp();
            if (place == NULL)
                return;
            if (place->kind == FROM_ARG)
                place->kind = WADDRESS;
            else
                place->kind = ADDRESS;
            place->type = t1->type->array.elem;
            InterCode code0_in = (InterCode)malloc(sizeof(struct InterCode_));
            code0_in->kind = ADD;
            code0_in->u.ubinop.result = place;
            code0_in->u.ubinop.op1 = t1;
            code0_in->u.ubinop.op2 = t2;
            add_to_intercode(code0_in);
        }

        return;
    }
    if (strcmp(fir_bro->Token, "Exp") == 0)
    {
        // Exp → LP Exp RP
        // printf("a translate_Exp Exp\n");
        translate_Exp(fir_bro, place);
        return;
    }
    if (strcmp(sec_bro->Token, "Args") == 0)
    {
        // Exp → ID LP Args RP
        //函数调用，有参数
        // 0.区分是否为read/write
        // printf("a translate_Exp Args\n");
        char *function_name = child->Id_Type;
        if (strcmp(function_name, "write") == 0)
        {
            // printf("a translate_Exp Args Write\n");
            Operand t1 = new_temp();
            translate_Exp(sec_bro->firstchild, t1);
            //要对地址类的取地址处理
            Operand t0 = NULL;
            // printf("a there\n");
            if (t1->kind == ADDRESS)
            {
                t0 = new_temp();
                InterCode code0_in = (InterCode)malloc(sizeof(struct InterCode_));
                code0_in->kind = ADDRASS2;
                code0_in->u.uassign.op1 = t0;
                code0_in->u.uassign.op2 = t1;
                add_to_intercode(code0_in);
            }
            // printf("a here\n");
            InterCode code1_in = (InterCode)malloc(sizeof(struct InterCode_));
            code1_in->kind = WRITE;
            if (t0 == NULL)
                code1_in->u.ulabel.op = t1;
            else
                code1_in->u.ulabel.op = t0;
            add_to_intercode(code1_in);
            return;
        }
        // 1.ARG传递参数，逆序
        translate_Args(sec_bro, NULL);
        // 1.2创建函数operand
        Operand func_ope = (Operand)malloc(sizeof(struct Operand_));
        func_ope->kind = FUNCTION;
        func_ope->u_char = function_name;
        // printf("a %s\n",function_name);
        // 2.place=CALL FUNCTION,保存返回值
        InterCode code1_in = (InterCode)malloc(sizeof(struct InterCode_));
        code1_in->kind = CALL;
        code1_in->u.uassign.op1 = place;
        code1_in->u.uassign.op2 = func_ope;
        add_to_intercode(code1_in);
        return;
    }
    if (strcmp(sec_bro->Token, "RP") == 0)
    {
        // Exp → ID LP RP
        //没有参数的函数调用
        // 0.区分是否为read/write
        // printf("a translate_Exp RP\n");
        char *function_name = child->Id_Type;
        if (strcmp(function_name, "read") == 0)
        {
            InterCode code1_in = (InterCode)malloc(sizeof(struct InterCode_));
            code1_in->kind = READ;
            code1_in->u.ulabel.op = place;
            add_to_intercode(code1_in);
            return;
        }
        // 1.2创建函数operand
        Operand func_ope = (Operand)malloc(sizeof(struct Operand_));
        func_ope->kind = FUNCTION;
        func_ope->u_char = function_name;
        // printf("a %s\n",function_name);
        // 2.place=CALL FUNCTION,保存返回值
        InterCode code1_in = (InterCode)malloc(sizeof(struct InterCode_));
        code1_in->kind = CALL;
        code1_in->u.uassign.op1 = place;
        code1_in->u.uassign.op2 = func_ope;
        add_to_intercode(code1_in);
        return;
    }
}

void translate_Cond(P_Node now, Operand lt, Operand lf)
{
    // printf("a translate_Cond\n");
    if (interim_is_error == 1)
        return;
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
        code3->u.uif.x = t1;
        code3->u.uif.relop = op;
        code3->u.uif.y = t2;
        code3->u.uif.z = lt;
        add_to_intercode(code3);
        // goto labelfalse
        InterCode code4 = (InterCode)malloc(sizeof(struct InterCode_));
        code4->kind = GOTO;
        code4->u.ulabel.op = lf;
        add_to_intercode(code4);
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
        code1->u.ulabel.op = l1;
        add_to_intercode(code1);
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
        code1->u.ulabel.op = l1;
        add_to_intercode(code1);
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
        code2->u.uif.x = t1;
        code2->u.uif.relop = op;
        code2->u.uif.y = t2;
        code2->u.uif.z = lt;
        add_to_intercode(code2);
        // goto
        InterCode code4 = (InterCode)malloc(sizeof(struct InterCode_));
        code4->kind = GOTO;
        code4->u.ulabel.op = lf;
        add_to_intercode(code4);
        // printf("a out1\n");
    }
    return;
}

void translate_Dec(P_Node now)
{
    // Dec → VarDec| VarDec ASSIGNOP Exp
    // printf("a translate_Dec\n");
    if (interim_is_error == 1)
        return;
    P_Node vardec = now->firstchild;
    P_Node fir_bro = vardec->nextbro;
    if (fir_bro == NULL)
    {
        Operand t1 = new_temp();
        translate_VarDec(vardec, t1);
    }
    else
    {
        Operand rem1 = new_temp();
        Operand rem2 = new_temp();
        translate_VarDec(vardec, rem1);
        translate_Exp(fir_bro->nextbro, rem2);

        InterCode func_in = (InterCode)malloc(sizeof(struct InterCode_));
        func_in->kind = ASSIGN;
        func_in->u.uassign.op1 = rem1;
        func_in->u.uassign.op2 = rem2;
        add_to_intercode(func_in);
    }
}

void translate_VarDec(P_Node now, Operand place)
{
    // VarDec → ID| VarDec LB INT RB
    //会生成DEC语句
    //只会出现一维数组，所以在else语句中直接处理数组
    // printf("a translate_VarDec\n");
    if (interim_is_error == 1)
        return;
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
            func_in->u.udec.op = place;
            func_in->u.udec.size = get_size(find_field->type);
            add_to_intercode(func_in);
            return;
        }
        // printf("a isERROR\n");
    }
    else
    {
        //只能是一维数组
        // 1.出现多维数组，输出error
        if (strcmp(now->firstchild->firstchild->Token, "ID") != 0)
        {
            interim_is_error = 1;
            printf("Cannot translate: Code contains variables of error array type.\n");
            return;
        }
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
        func_in->u.udec.op = place;
        func_in->u.udec.size = get_size(find_it->field->type);
        add_to_intercode(func_in);
        return;
    }
}

void translate_Args(P_Node now, InterCode here)
{
    //传递的如果是数组、结构体，是一个address类型
    //逆序
    // Args → Exp COMMA Args| Exp
    // printf("a translate_Args\n");
    if (interim_is_error == 1)
        return;

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
    code1_in->u.ulabel.op = t1;
    // 3.连接
    if (here == NULL)
    {
        code1_in->next = NULL;
        code1_in->before = NULL;
    }
    else
    {
        here->next = code1_in;
        code1_in->before = here;
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
            temp = temp->before;
            add_to_intercode(rem);
        }
    }
}

void translate_Specifier(P_Node now)
{
    // Specifier → TYPE| StructSpecifier
    // printf("a translate_Specifier\n");
    if (interim_is_error == 1)
        return;
    if (strcmp(now->firstchild->Token, "StructSpecifier") == 0)
    {
        translate_StructSpecifier(now->firstchild);
    }
}

void translate_StructSpecifier(P_Node now)
{
    // StructSpecifier → STRUCT OptTag LC DefList RC| STRUCT Tag
    // printf("a translate_StructSpecifier\n");
    if (interim_is_error == 1)
        return;
    P_Node struct_s = now->firstchild;
    P_Node fir_bro = struct_s->nextbro;
    P_Node sec_bro = fir_bro->nextbro;
    if (sec_bro == NULL)
    {
        // Tag → ID,生成DEC语句
        Operand t1 = (Operand)malloc(sizeof(struct Operand_));
        Vari find = find_vari_table(fir_bro->firstchild->Id_Type);
        t1->kind = VARIABLE;
        t1->u_int = find->variable;
        t1->u_char = fir_bro->firstchild->Id_Type;
        // printf("a create_dec1\n");
        InterCode func_in = (InterCode)malloc(sizeof(struct InterCode_));
        func_in->kind = DEC;
        func_in->u.udec.op = t1;
        func_in->u.udec.size = get_size(find->field->type);
        add_to_intercode(func_in);
        return;
    }
}

void add_to_intercode(InterCode this)
{
    // translate_print_test(this);
    // if(in_now!=NULL) translate_print_test(in_now);
    // printf("\n");
    this->before = NULL;
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
        this->before = in_now;
        in_now = this;
    }
}

Operand new_temp()
{
    Operand now = (Operand)malloc(sizeof(struct Operand_));
    now->kind = TEMP;
    now->u_int = temp_num;
    now->u_char = "zhangyu";
    temp_num++;
    return now;
}

Operand new_label()
{
    Operand now = (Operand)malloc(sizeof(struct Operand_));
    now->kind = LABEL;
    now->u_int = lab_num;
    lab_num++;
    return now;
}

//计算部分偏移
int get_offset(Type return_type, P_Node after)
{
    // printf("a get_offset\n");
    if (return_type == NULL)
        return 0; // printf("a NULL\n");
    if (return_type->type == BASIC)
        return 4;
    else if (return_type->type == STRUCT)
    {
        // printf("a get_offset STRUCT\n");
        // printf("a %s\n",after->Id_Type);
        int offset = 0;
        FieldList count = return_type->structure;
        while (count != NULL)
        {
            if (strcmp(count->name, after->Id_Type) == 0)
                break;
            offset = offset + get_size(count->type);
            count = count->next;
        }
        return offset;
    }
    else if (return_type->type == ARRAY)
    {
        //只有一维数组，多出一维都报错
        // printf("a get_offset ARRAY\n");
        if (return_type->array.elem->type == ARRAY)
        {
            interim_is_error = 1;
            printf("Cannot translate: Code contains variables of error array type.\n");
            return -1;
        }
        //返回该数组单个元素的长度
        int offset = 0;
        Type elem = return_type->array.elem;
        offset = get_size(elem);
        return offset;
    }
}

//计算全部偏移，即type中的所有内存都要加上
int get_size(Type type)
{
    // printf("a get_size\n");
    if (type == NULL)
        return 0;
    if (type->type == BASIC)
        return 4;
    //只有一维数组
    else if (type->type == ARRAY)
    {
        // printf("a get_size ARRAY\n");
        int temp = type->array.size * get_size(type->array.elem);
        return temp;
    }
    else if (type->type == STRUCT)
    {
        // printf("a get_size STRUCT\n");
        int offset = 0;
        FieldList count = type->structure;
        while (count != NULL)
        {
            offset = offset + get_size(count->type);
            count = count->next;
        }
        return offset;
    }
}
