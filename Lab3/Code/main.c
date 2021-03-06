#include <stdlib.h>
#include <stdio.h>
#include "node.h"
#include "semantics.h"
#include "interim.h"

extern FILE* yyin;
extern P_Node root;

void yyrestart (FILE *input_file);
int yyparse(void);

int hasFault = FALSE;

int main(int argc, char** argv)
{
    if (argc <= 1) return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f){
        perror(argv[1]);  //把一个描述性错误消息输出到标准错误 stderr
        return 1;
    }
    FILE* f2=fopen(argv[2],"w+");
    if(!f2)
    {
        perror(argv[2]);
        return 1;
    }
    yyrestart(f); //初始化输入文件指针yyin
    yyparse(); //语法分析
    if (!hasFault){
        // Traverse_Print(root, 0);
        semantic_analysis(root);  // 语义分析
        translate_Program(root,f2); //中间代码生成
    }
    return 0;
}



