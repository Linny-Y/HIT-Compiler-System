#include <stdlib.h>
#include <stdio.h>
#include "node.h"
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
    yyrestart(f); //初始化输入文件指针yyin
    yyparse(); //语法分析
    if (!hasFault){
        Traverse_Print(root, 0);
    }
    return 0;
}

/*
int main(int argc, char** argv){
    if (argc > 1)
    {
        if (!(yyin=fopen(argv[1], "r"))){
            perror(argv[1]); //把一个描述性错误消息输出到标准错误 stderr
            return 1;
        }
    }
    while (yylex() != 0);
    return 0;
}
*/


