#include "semantics.h"


Var vartable[SIZE+1];//储存结构体或变量
int struct_without_name=0;


unsigned int hash_pjw(char* name){
    unsigned int val=0,i;
    for(;*name;++name){
        val=(val<<2)+*name;
        if(i=val*~SIZE){
            val=(val^(i>>12))&SIZE; //16384
        }
    }
    return val;
}
// check if a name is in the table
Var find_var_table(char* name){
    unsigned int val = hash_pjw(name);
    Var x = vartable[val];
    while (x != NULL) {
        if (strcmp(x->field->name, name) == 0)
            break;
        x = x->next;
    }
    return x;
}
Var create_var(int in_struct, FieldList field, Var next, int line) {
    Var var_table = (Var)malloc(sizeof(struct Var_));
    var_table->is_def_struct = in_struct;
    var_table->field = field;
    var_table->next = next;
    var_table->line = line;
    return var_table;
}
// add a field to the table
void insert_var_table(FieldList field_list, int line, int in_struct){
    int val = hash_pjw(field_list->name);
    if (vartable[val] == NULL) {
        vartable[val] = create_var(in_struct, field_list, NULL, line);
        return;
    }
    Var now = vartable[val];
    while (now->next != NULL) {
        now = now->next;
    }
    now->next = create_var(in_struct, field_list, NULL, line);
}

// compare two types
int typecmp(Type a, Type b){
    if (a == NULL && b == NULL)
        return 0;
    if (a == NULL || b == NULL)
        return 1;
    if (a->type != b->type)
        return 1;
    if (a->type == BASIC)
        return a->basic != b->basic;
    if (a->type == ARRAY)
        return typecmp(a->array.elem, b->array.elem);
    if (a->type == STRUCT)
        return fieldcmp(a->structure, b->structure, 1);
}

int fieldcmp(FieldList a,FieldList b, int compare_name) {
    if (a == NULL && b == NULL)
        return 0;
    if (a == NULL || b == NULL)
        return 1;
    if (compare_name && strcmp(a->name, b->name))
        return 1;
    if (typecmp(a->type, b->type))
        return 1;
    return fieldcmp(a->next, b->next, compare_name);
}

// 建立变量表 查找错误
void semantic_analysis(P_Node now){
    for(int i=0;i<SIZE;i++){
        vartable[i]=NULL;
    }
    // Program -> ExtDefList
    P_Node extdeflist=now->firstchild;
    while(extdeflist->firstchild!=NULL){
        // ExtDefList -> ExtDef ExtDefList| empty
        P_Node extdef=extdeflist->firstchild;
        ExtDef(extdef);
        extdeflist=extdef->nextbro;
    }
}
void ExtDef(P_Node now){
    /*
    ExtDef → Specifier ExtDecList SEMI
    | Specifier SEMI
    | Specifier FunDec CompSt 函数
    */
   P_Node specifier=now->firstchild;
   Type specifier_type=Specifier(specifier);
   //在使用type时，要注意type==null的情况
   P_Node fir_bro=specifier->nextbro;
   P_Node sec_bro=fir_bro->nextbro;
   if(sec_bro==NULL){
        //ExtDef → Specifier SEMI
        return;
   }
   else if(!strcmp(fir_bro->Token,"ExtDecList")){
        //ExtDef → Specifier ExtDecList SEMI
        //判断变量的名字是否被使用
        //根据specifier设定extdeclist变量类型
        ExtDecList(fir_bro,specifier_type);
   }
}

Type Specifier(P_Node now){
    //Specifier → TYPE| StructSpecifier
    P_Node child=now->firstchild;
    if(!strcmp(child->Token,"TYPE")){
        // 设定变量类型
        Type return_type=(Type)malloc(sizeof(struct Type_));
        return_type->type=BASIC;
        if(!strcmp(child->Id_Type,"int")){
            return_type->basic=TYPE_INT;
        }
        else{
            return_type->basic=TYPE_FLOAT;
        }
        return return_type;
    }
    else{
        Type return_type=StructSpecifier(child);
        return return_type;
    }
}
Type StructSpecifier(P_Node now){
    //StructSpecifier → STRUCT OptTag LC DefList RC
    //                | STRUCT Tag
    P_Node struct_token=now->firstchild;
    P_Node optTag=struct_token->nextbro;
    P_Node defList=optTag->nextbro->nextbro;
    
    if (!strcmp(optTag->Token,"OptTag")){
        //OptTag → ID| empty
        FieldList struct_field=(FieldList)malloc(sizeof(struct FieldList_));
        Type type=(Type)malloc(sizeof(struct Type_));
        type->type=STRUCT;
        type->structure = DefList(defList,DEF_STRUCT);
        struct_field->name = OptTag(optTag);
        struct_field->type = type;
        struct_field->next = NULL;
        if (struct_field->name == NULL)
            return NULL;
        insert_var_table(struct_field, now->firstchild->line, DEF_IN_STRUCT);
        type->structure = struct_field;
        return type;
    }else {
        // StructSpecifier -> STRUCT Tag
        // Tag -> ID
        Var new = find_var_table(struct_token->nextbro->firstchild->Id_Type);
        return new->field->type;
    }
}
char* OptTag(P_Node now){
    if (now->Token == NULL) {
        // OptTag -> empty
        char name[32];
        sprintf(name, "%s%d", "unnamed_struct", struct_without_name++);
        return strdup(name);
    } 
    else {
        // OptTag -> ID
        return now->firstchild->Id_Type;
    }
}

FieldList DefList(P_Node now,int judge){
    //DefList → Def DefList| empty
    P_Node newdef=now->firstchild;
    FieldList head=NULL;
    FieldList nownode=NULL;
    while(newdef!=NULL){
        FieldList nxt=Def(newdef,judge);
        if(head==NULL) {
            head=nownode=nxt;
        }
        else {
            nownode->next=nxt;
            nownode=nxt;
        }
        if(nownode==NULL) break;
        while(nownode->next!=NULL) nownode=nownode->next;
        newdef=newdef->nextbro->firstchild;
    }
    return head;  
}

FieldList Def(P_Node now,int judge){
    //Def → Specifier DecList SEMI
    P_Node declist=now->firstchild->nextbro;
    return DecList(declist,Specifier(now->firstchild),judge);
}

FieldList DecList(P_Node now,Type type,int judge){
    //DecList → Dec| Dec COMMA DecList
    //a,b,c
    P_Node dec=now->firstchild;
    P_Node fir_bro=dec->nextbro;
    if(fir_bro==NULL){
        //DecList → Dec
        return Dec(dec,type,judge);
    }
    else{
        //DecList → Dec COMMA DecList
        FieldList fieldlist=Dec(dec,type,judge);
        if(fieldlist!=NULL)
            fieldlist->next=DecList(dec->nextbro->nextbro,type,judge);
        return fieldlist;
    }
}

FieldList Dec(P_Node now,Type type,int judge){
    //Dec → VarDec| VarDec ASSIGNOP Exp
    P_Node vardec=now->firstchild;
    P_Node fir_bro=vardec->nextbro;
    if(fir_bro==NULL){
        //Dec → VarDec
        return VarDec(vardec,type,judge);
    }
    else{
        //Dec → VarDec ASSIGNOP Exp
        if(judge==DEF_STRUCT){
            printf("Error type 15 at Line %d: Initialize domain in structure.\n",vardec->line);
            return NULL;
        }
        Type exp_type=Exp(fir_bro->nextbro);
        if(typecmp(type,exp_type)==1){
            printf("Error type 5 at Line %d: Type mismatched for assignment.\n",now->line);
            return NULL;
            
        }
        return VarDec(vardec,type,judge);
    }
}

FieldList VarDec(P_Node now,Type type,int judge){
    //VarDec → ID| VarDec LB INT RB
    P_Node child=now->firstchild;
    P_Node fir_bro=child->nextbro;
    if(fir_bro==NULL){
        //VarDec → ID
        FieldList fieldlist=(FieldList)malloc(sizeof(struct FieldList_));
        fieldlist->name=child->Id_Type;
        fieldlist->type=type;
        fieldlist->next=NULL;

        if (find_var_table(fieldlist->name)){
            if (judge == DEF_STRUCT){
                printf("Error type 15 at Line %d: Redefined structure's domain \"%s\".\n",now->line,child->Id_Type);
                return NULL;
            }
            else if (judge == DEF_VAR) {
                printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", now->line, now->firstchild->Id_Type);
                return NULL;
            }
        }
        insert_var_table(fieldlist,child->line,DEF_NOT_IN_STRUCT);
        return fieldlist;
    }
    else{
        P_Node int_token=fir_bro->nextbro;
        Type arraytype=(Type)malloc(sizeof(struct Type_));
        arraytype->type=ARRAY;
        arraytype->array.size=int_token->intVal;
        arraytype->array.elem=type;

        return VarDec(child,arraytype,judge);
    }
}

void ExtDecList(P_Node now,Type type){
    //ExtDecList → VarDec| VarDec COMMA ExtDecList
    P_Node vardec=now->firstchild;
    P_Node fir_bro=vardec->nextbro;
    VarDec(vardec,type,DEF_VAR);
    if(!(fir_bro==NULL)){
        //ExtDecList → VarDec COMMA ExtDecList
        ExtDecList(fir_bro->nextbro,type);
    }
}
FieldList VarList(P_Node now,int judge){
    //VarList → ParamDec COMMA VarList| ParamDec
    P_Node paramdec=now->firstchild;
    P_Node fir_bro=paramdec->nextbro;
    if(fir_bro==NULL){
        //VarList → ParamDec
        return ParamDec(paramdec,judge);
    }
    else{
        //VarList → ParamDec COMMA VarList
        FieldList fieldlist=ParamDec(paramdec,judge);
        if(fieldlist!=NULL)
            fieldlist->next=VarList(fir_bro->nextbro,judge);
        return fieldlist; 
    }
}

FieldList ParamDec(P_Node now,int judge){
    //ParamDec → Specifier VarDec
    Type specifier_type=Specifier(now->firstchild);
    return VarDec(now->firstchild->nextbro,specifier_type,judge);
}

Type Exp(P_Node now){
    //printf("aExp\n");
    P_Node child=now->firstchild;
    P_Node fir_bro=child->nextbro;
    if(fir_bro==NULL){
        // INT ID FLOAT
        Type return_type=(Type)malloc(sizeof(struct Type_));
        return_type->type=BASIC;
        if(strcmp(child->Token,"INT")==0){
            //Exp → INT
            return_type->basic=TYPE_INT;
            return return_type;
        }
        else if(strcmp(child->Token,"FLOAT")==0){
            //Exp → FLOAT
            return_type->basic=TYPE_FLOAT;
            return return_type;
        }
        else if(strcmp(child->Token,"ID")==0){
            //Exp → ID
            //printf("aExp → ID\n");
            Var find_id=find_var_table(child->Id_Type);
            if(find_id==NULL|| find_id->is_def_struct == DEF_IN_STRUCT){
                printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",now->line,child->Id_Type);
                return NULL;
            }
            return find_id->field->type;
        }
    }
    if(fir_bro->nextbro==NULL){
        P_Node exp = fir_bro;
        if(strcmp(child->Token,"MINUS")==0){
            //Exp → MINUS Exp
            return Exp(exp);
        }
        else if(strcmp(child->Token,"NOT")==0){
            //Exp →NOT Exp
            Type return_type=Exp(exp);
            if(return_type==NULL)
                return NULL;
            if(return_type->type==BASIC&&return_type->basic==TYPE_INT)
                return return_type;
            
            printf("Error type 7 at Line %d: Type mismatched for operands.\n",now->line);
            return NULL;
            
        }
    }
    if(strcmp(fir_bro->Token,"ASSIGNOP")==0){
        //Exp → Exp ASSIGNOP Exp
        //child->ID | Exp LB Exp RB | Exp DOT ID
        P_Node child_child=child->firstchild;
        if(child_child==NULL) return NULL;
        else if(strcmp(child_child->Token,"ID")==0&&child_child->nextbro==NULL);
        else if(strcmp(child_child->Token,"Exp")==0&&strcmp(child_child->nextbro->Token,"LB")==0);
        else if(strcmp(child_child->Token,"Exp")==0&&strcmp(child_child->nextbro->Token,"DOT")==0);
        else{
            printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",now->line);
            return NULL;
        }
        Type left_type=Exp(child);
        Type right_type=Exp(fir_bro->nextbro);
        if(typecmp(left_type,right_type)==0) return right_type;
        else {
            printf("Error type 5 at Line %d: Type mismatched for assignment.\n",now->line);
            return NULL;
        }
    }
    if(strcmp(fir_bro->Token,"AND")==0||strcmp(fir_bro->Token,"OR")==0){
        //Exp → Exp AND Exp | Exp OR Exp
        Type left_type=Exp(child);
        Type right_type=Exp(fir_bro->nextbro);
        //逻辑操作与
        if(left_type==NULL&&right_type==NULL) return NULL;
        if(typecmp(left_type,right_type)==0&&left_type->type==BASIC&&left_type->basic==0) return right_type;
        else {
            printf("Error type 7 at Line %d: Type mismatched for operands.\n",now->line);
            return NULL;
        }
    }
    if(strcmp(fir_bro->Token,"RELOP")==0||strcmp(fir_bro->Token,"PLUS")==0||
        strcmp(fir_bro->Token,"MINUS")==0||strcmp(fir_bro->Token,"STAR")==0||
        strcmp(fir_bro->Token,"DIV")==0){
        Type left_type=Exp(child);
        Type right_type=Exp(fir_bro->nextbro);
        if(typecmp(left_type,right_type)==0&&left_type!=NULL&&left_type->type==BASIC) return right_type;
        else {
            printf("Error type 7 at Line %d: Type mismatched for operands.\n",now->line);
            return NULL;
        }
    }
    if(strcmp(fir_bro->Token,"DOT")==0){
        //Exp → Exp DOT ID
        Type return_type=Exp(child);
        if(return_type==NULL) return NULL;
        if(return_type->type==STRUCT);
        else {
            printf("Error type 13 at Line %d: Illegal use of \".\".\n",now->line);
            return NULL;
        }
    }
    if(strcmp(fir_bro->Token,"LB")==0){
        //Exp → Exp LB Exp RB
        Type left_type=Exp(child);
        if(left_type==NULL) return NULL;
        if(left_type->type!=ARRAY){
            printf("Error type 10 at Line %d: Not array.\n",child->line);
            return NULL;
        }
        Type right_type=Exp(child->nextbro->nextbro);
        if(right_type==NULL) return NULL;
        if(right_type->type==BASIC&&right_type->basic==TYPE_INT){
            return left_type->array.elem;
        }
        else{
            // [float]
            printf("Error type 12 at Line %d: Not integer.\n",child->line);
            return NULL;
        }
    }
    if(strcmp(child->Token,"LP")==0){
        //Exp → LP Exp RP
        return Exp(fir_bro);
    }
}
// void CompSt(P_Node now,Type type){
//     //CompSt → LC DefList StmtList RC
//     P_Node deflist=now->firstchild->nextbro;
//     P_Node stmtlist=deflist->nextbro;

//     DefList(deflist,DEF_VAR);
//     StmtList(stmtlist,type);
// }

// void StmtList(P_Node now,Type type){
//     //StmtList → Stmt StmtList| empty
//     P_Node child=now->firstchild;
//     if(now==NULL)
//         return;
//     Stmt(child,type);
//     StmtList(child->nextbro,type);
// }

// void Stmt(P_Node now,Type type){
//     P_Node child=now->firstchild;
//     if(strcmp(child->Token,"Exp")==0){
//         //Stmt → Exp SEMI
//         Exp(child);
//     }
//     else if(strcmp(child->Token,"CompSt")==0){
//         //Stmt →CompSt
//         CompSt(child,type);
//     }
//     // else if(strcmp(child->Token,"RETURN")==0){
//     //     //Stmt →RETURN Exp SEMI
//     //     Type exp_type=Exp(child->nextbro);
//     //     if(typecmp(exp_type,type)==1){
//     //         printf("Error type 8 at Line %d: Type mismatched for return.\n",now->line);
//     //         return;
//     //     }
//     // }
//     else if(strcmp(child->Token,"IF")==0){
//         //Stmt →IF LP Exp RP Stmt|IF LP Exp RP Stmt ELSE Stmt
//         P_Node exp_in_if=child->nextbro->nextbro;
//         Type exp_type=Exp(exp_in_if);
//         if(exp_type==NULL) return;
//         if(exp_type->type!=BASIC||exp_type->basic!=TYPE_INT){
//             printf("Error type 7 at Line %d: Type mismatched for operands.\n",exp_in_if->line);
//             return;
//         }
//         P_Node stmt = exp_in_if->nextbro->nextbro;
//         Stmt(stmt,type);
//         if(!(stmt->nextbro==NULL)){
//             Stmt(stmt->nextbro->nextbro,type);
//         }
//     }
//     else if(strcmp(child->Token,"WHILE")==0){
//         //Stmt →WHILE LP Exp RP Stmt
//         P_Node exp_in_while=child->nextbro->nextbro;
//         Type exp_type=Exp(exp_in_while);
//         if(exp_type==NULL) return;
//         if(exp_type->type!=BASIC||exp_type->basic!=TYPE_INT){
//             printf("Error type 7 at Line %d: Type mismatched for operands.\n",exp_in_while->line);
//             return;
//         }
//         Stmt(exp_in_while->nextbro->nextbro,type);
//     }   
// }