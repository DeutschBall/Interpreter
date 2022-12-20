#pragma once
#include <cstring>
#include "lexer/lexer.hpp"
#include <iostream>
using namespace std;
struct Node{//表达式树的节点
    TokenType type;//所有节点,不管是数值还是运算,都一个类型
    union {//匿名联合,因为外部不需要引用
        struct {
            Node *left;
            Node *right;
        }case_operator;
        struct {
            Node *child;
            Func func;
        }case_function;
        double case_const;
        double *case_parameter;//T时使用,记录参数
    }content;

    Node(TokenType t=NONTOKEN):type(t){memset(&content,0,sizeof(content));}
    void setCaseConst(const double &setter){
        content.case_const=setter;
    }
    void setCaseParameter(double * parameter){
        content.case_parameter=parameter;
    }
    void setCaseFunction(Func func,Node *child){
        content.case_function.func=func;
        content.case_function.child=child;
    }
    void setCaseOperator(Node *left,Node *right){
        content.case_operator.left=left;
        content.case_operator.right=right;
    }
    TokenType getType(){
        return type;
    }
    Node *getLeft(){
        return content.case_operator.left;
    }
    Node *getRight(){
        return content.case_operator.right;
    }
    Func getFunction(){
        return content.case_function.func;
    }
    Node* getChild(){
        return content.case_function.child;
    }
    double getConst(){
        return content.case_const;
    }
    double *getParameter(){
        return content.case_parameter;
    }

    friend std::ostream &operator<<(std::ostream &os,const Node &node){//打印检查方便调试
        os<<node.type;
        return os;
    }

};
static Token* current_token;//最近一次获取到的字符
extern void parser(void);
