#include <iostream>
#include <vector>

#include <algorithm>
#include "message/message.hpp"
#include "lexer/lexer.hpp"
#include "semantic/semantic.hpp"
#include "parser.hpp"


//前置声明

static Node *evalAtom(void);
static Node *evalComponent(void);
static Node *evalFactor(void);
static Node *evalTerm(void);
static Node *evalExpression(void);


void fetchToken(void){
    current_token=getToken();
    if(current_token->getType()==ERRORTOKEN){//lexer analization failed
        error("error at fetch token");
    }
}


void matchToken(TokenType expected_type){
    if(current_token->getType()!=expected_type){
        error("unexpected token:"+ current_token->toString());
    }
    else{
        // if(current_token!=NULL){
        //     delete current_token;
        // }
        // delete current_token;
        fetchToken();
    }
}


//实际上用不到makeExpNode,并且这个函数根据第一个参数类型决定后面两个参数类型,属于控制耦合,感觉上太暴力了
// /**
//  * @brief 根据opcode类型决定param1和param2的意义,建立一个表达式树节点,返回其指针
//  * @param opcode CONST_ID,T,FUNC,default
//  * @param param1 常数,参数指针,函数指针,左子节点指针
//  * @param param2 无意义,无意义,子节点指针,右子节点指针
//  * @return 返回Node*指针
//  */
// static Node * makeExpNode(TokenType opcode,void *param1=NULL,void *param2=NULL){
//     Node *root=new Node(opcode);
//     switch(opcode){
//         case CONST_ID:
//             root->setCaseConst(* (double*)param1);
//             break;
//         case T:
//             root->setCaseParameter((double*)param1);
//             break;
//         case FUNC:
//             root->setCaseFunction((Func)param1,(Node*)param2);
//             break;
//         default:
//             root->setCaseOperator((Node*)param1,(Node*)param2);
//     }
//     return root;
// }


/**
 * @brief Atom->CONST_ID | T
 * 
 * @return Node* 
 */
static Node* evalAtom(void){//最底层的文法
    Token *token;
    Node *root;
    Node *expression;
    token=current_token;
    switch(token->getType()){
        case CONST_ID:
            matchToken(CONST_ID);
            root=new Node(CONST_ID);
            root->setCaseConst(token->getValue());
            break;
        case T:
            matchToken(T);
            root=new Node(T);
            root->setCaseParameter(&token->value);
            break;
        case FUNC:
            matchToken(FUNC);
            matchToken(L_BRACKET);
            expression=evalExpression();
            root=new Node(FUNC);
            root->setCaseFunction(token->getFunc(),expression);
            matchToken(R_BRACKET);
            break;
        case L_BRACKET:
            matchToken(L_BRACKET);
            expression=evalExpression();
            root=expression;
            matchToken(R_BRACKET);
            break;
        default:
            error("Invalid expression at line "+std::to_string(line_no));
            break;
    }
    return root;
}



/**
 * @brief Component->Atom POWER Component | Atom
 * 
 * @return Node* 
 */
static Node *evalComponent(void){
    Node *left;
    Node *right;
    Node *root;
    left=evalAtom();
    root=left;
    if(current_token->getType()==POWER){
        matchToken(POWER);
        right=evalComponent();
        root=new Node(POWER);
        root->setCaseOperator(left,right);
    }
    return root;
}


/**
 * @brief Factor->+Factor | -Factor | Component
 * 
 * @return Node* 
 */
static Node *evalFactor(void){
    Node *left;
    Node *right;
    Node *root;
    TokenType type=current_token->getType();
    switch(type){
        case PLUS:case MINUS:
            matchToken(type);
            right=evalFactor();
            left=new Node(CONST_ID);
            left->setCaseConst(0.0);
            root=new Node(type);
            root->setCaseOperator(left,right);
            break;
        default:
            root=evalComponent();
            break;
    }
    return root;
}

/**
 * @brief Term->Factor {*Factor} | Factor {/Factor}
 * 
 * @return Node* 
 */
static Node *evalTerm(void){
    Node *left;
    Node *right;
    Node *root;
    TokenType type;
    left=evalFactor();
    root=left;
    while(current_token->getType()==MUL||current_token->getType()==DIV){
        type=current_token->getType();
        matchToken(type);
        right=evalFactor();
        root=new Node(type);
        root->setCaseOperator(left,right);
        left=root;
    }
    return root;
}

/**
 * @brief Expression->Term {+Term} | Term {-Term}
 * 
 * @return Node* 
 */
static Node *evalExpression(void){
    Node *left;
    Node *right;
    Node *root;

    TokenType type;
    left=evalTerm();
    root=left;
    while(current_token->getType()==PLUS||current_token->getType()==MINUS){
        type=current_token->getType();
        matchToken(type);
        right=evalTerm();
        root=new Node(type);
        root->setCaseOperator(left,right);
        left=root;
    }
    return root;
}


/**
 * @brief 已经求过值的分析树没用了就扬了
 * 
 * @param root 
 */
void deleteTree(Node *root){
    if(root==NULL){
        return ;
    }
    switch(root->getType()){
        case PLUS:case MINUS:case MUL:case DIV:case POWER:
            deleteTree(root->getLeft());
            deleteTree(root->getRight());
            break;
        case FUNC:
            deleteTree(root->getChild());
            break;
        default:
            break;
    }
    delete root;
    root=NULL;
}

/**
 * @brief ORIGIN IS (Expression,Expression)
 * 
 */
void originStatement(void){
    Node *root;
    matchToken(ORIGIN);
    matchToken(IS);
    matchToken(L_BRACKET);

    root=evalExpression();
    origin_x=getExpValue(root);
    deleteTree(root);
    
    matchToken(COMMA);
    
    root=evalExpression();
    origin_y=getExpValue(root);
    delete root;

    matchToken(R_BRACKET);
}

/**
 * @brief SCALE IS (Expression,Expression)
 * 
 */
void scaleStatement(void){
    Node *root;

    matchToken(SCALE);
    matchToken(IS);
    matchToken(L_BRACKET);

    root=evalExpression();
    scale_x=getExpValue(root);
    delete(root);

    matchToken(COMMA);

    root=evalExpression();
    scale_y=getExpValue(root);
    delete(root);

    matchToken(R_BRACKET);

}

/**
 * @brief ROT IS Expression
 * 
 */
void rotStatement(void){
    Node *root;
    matchToken(ROT);
    matchToken(IS);
    root=evalExpression();
    rot=getExpValue(root);
    deleteTree(root);
}

/**
 * @brief T IS Expression
 * 
 */
void varStatement(void){
    Node *root;
    Token *pToken=current_token;
    matchToken(T);
    matchToken(IS);
    root=evalExpression();
    pToken->setValue(getExpValue(root));
    deleteTree(root);
}

/**
 * @brief FOR T FROM Expression TO Expression STEP Expression DRAW (Expression,Expression)
 * 
 */
void forStatement(void){
    
    Node *root;
    Token *pToken;
    double start;
    double end;
    double step;
    Node *x;
    Node *y;

    matchToken(FOR);
    pToken=current_token;
    matchToken(T);
    matchToken(FROM);
    root=evalExpression();
    start=getExpValue(root);
    deleteTree(root);

    matchToken(TO);
    root=evalExpression();
    end=getExpValue(root);
    deleteTree(root);

    matchToken(STEP);
    root=evalExpression();
    step=getExpValue(root);
    deleteTree(root);

    matchToken(DRAW);
    matchToken(L_BRACKET);
    x=evalExpression();
    matchToken(COMMA);
    y=evalExpression();
    matchToken(R_BRACKET);

    draw(pToken,1.0*start,1.0*end,1.0*step,x,y);
    deleteTree(x);
    deleteTree(y);
}





void parser(void){
    fetchToken();//需要先获取一个符号,一是判断是否结束,而是每个循环都负责准备下一个循环的首符号,那么第一个循环需要手动给一个符号
    while(current_token->getType()!=NONTOKEN){//以分号结尾的行为单位进行循环
        switch (current_token->getType()){
        
            case ORIGIN:
                originStatement();
                break;
            case SCALE:
                scaleStatement();
                break;
            case ROT:
                rotStatement();
                break;
            case FOR:
                forStatement();
                break;
            // case T:
            //     varStatement();
            //     break;
            case SEMICO:
                break;
            default:
                error("unknown token");
                break;
        }
        matchToken(SEMICO);
    }
}