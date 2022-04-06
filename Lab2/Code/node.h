#ifndef _NODE_H_
#define _NODE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#define TRUE 1
#define FALSE 0
// 行数
extern int yylineno;

// 文本
extern char* yytext;

extern int hasFault;
// 错误处理
int yyerror(char *msg);

// bison是否有词法语法错误

/* 抽象语法树 */
struct treeNode{
    int line;  /* 行数 */
    
    char* Token;// Token类型
    
    int isleaf; // 为叶节点 即为终结符
    // 联合体，同一时间只能保存一个成员的值，分配空间是其中最大类型的内存大小
    union{
        // ID内容 TYPE类型
        char* Id_Type;  // = int/float
        // 具体的数值
        int intVal;  // INT
        float floatVal;  // FLOAT
    };

    // firstchild是第一个孩子节点，nextbro是兄弟节点，使用孩子兄弟表示法 用二叉树表示多叉树
    struct treeNode *firstchild,*nextbro;

};
typedef struct treeNode* P_Node;

//语法生成式为空的节点
static P_Node EmptyNode(){
    P_Node null = (P_Node)malloc(sizeof(struct treeNode));
    null->Token = NULL;
    return null;
}

//建立新节点
static P_Node NewTreeNode(int line, char* TOKEN, int amount, ...){
    if(hasFault) return NULL; // 存在错误，退出，不建立节点
    P_Node root = (P_Node)malloc(sizeof(struct treeNode));
    P_Node child = (P_Node)malloc(sizeof(struct treeNode));
    if (!root){
        yyerror("create treenode error");
        exit(0);
    }
    root->line = line;
    root->Token = TOKEN;
    root->isleaf = FALSE;
    if (amount == 0){
        //表示当前节点是终结符（叶节点）
        root->isleaf = TRUE;
        if ((!strcmp(TOKEN, "ID"))||(!strcmp(TOKEN, "TYPE"))){ // strcmp()==0 表示相同
            char *str;
            str = (char *)malloc(sizeof(char) * 40);
            strcpy(str, yytext);
            root->Id_Type = str;
        }
        else if(!strcmp(TOKEN, "INT")){
            root->intVal = atoi(yytext);
        }
        else if(!strcmp(TOKEN, "FLOAT")){
            root->floatVal = atof(yytext);
        }
    }
    else if(amount > 0){
        // 参数列表，详见 stdarg.h 用法
        va_list list;
        // 初始化参数列表
        va_start(list, amount);
        //第一个孩子
        child = va_arg(list,P_Node);
        root->firstchild = child;
        if (amount >= 2){
            // 存在两个以上的子节点，即第一个孩子有兄弟
            for (int i = 0; i < (amount - 1); i++){
                child->nextbro = va_arg(list,P_Node);
                child = child->nextbro;
            }
        }
        va_end(list);
    }
    return root;
}
//递归遍历语法树, height从 0 开始
static void Traverse_Print(P_Node tree, int height){
    if (tree != NULL && tree->Token != NULL){
        printf("%*s", 2 * height, ""); // 缩进两个空格
        printf("%s", tree->Token);  // 打印名称
        if ((!strcmp(tree->Token, "ID"))||(!strcmp(tree->Token, "TYPE"))){
            printf(": %s", tree->Id_Type);
        }
        else if(!strcmp(tree->Token, "INT")){
            printf(": %d", tree->intVal);
        }
        else if(!strcmp(tree->Token, "FLOAT")){
            printf(": %f", tree->floatVal);
        }
        else if (!tree->isleaf){ // 非终结符
            printf(" (%d)", tree->line);
        }
        printf("\n");
        Traverse_Print(tree->firstchild, height + 1);
        Traverse_Print(tree->nextbro, height);
    }
}

#endif