#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include <vector>
struct Token;
//对外只提供四个接口
extern int line_no;
extern Token* getToken(void);
extern bool initLexer(std::string filepath);
extern void closeLexer(void);

typedef double (*Func)(double);

enum TokenType
{
    ORIGIN,
    SCALE,
    ROT,
    IS,
    TO,
    STEP,
    DRAW,
    FOR,
    FROM,
    T,
    SEMICO,
    L_BRACKET,
    R_BRACKET,
    COMMA,
    PLUS,
    MINUS,
    MUL,
    DIV,
    POWER,
    FUNC,
    CONST_ID,
    NONTOKEN,
    ERRORTOKEN
};
static std::string STokenType[]={//使用TokenType作为下标,获取相应字符串
    "ORIGIN",
    "SCALE",
    "ROT",
    "IS",
    "TO",
    "STEP",
    "DRAW",
    "FOR",
    "FROM",
    "T",
    "SEMICO",
    "L_BRACKET",
    "R_BRACKET",
    "COMMA",
    "PLUS",
    "MINUS",
    "MUL",
    "DIV",
    "POWER",
    "FUNC",
    "CONST_ID",
    "NONTOKEN",
    "ERRORTOKEN"
};


/**
 * @brief 符号类型,Token(type,lexme,value,func)
 * 
 */
struct Token
{
    TokenType type;
    std::string lexme;
    double value;
    Func func;
    Token(TokenType t = ERRORTOKEN, std::string l = "", double v = 0.0, Func f = NULL) : type(t),
                                                                                         lexme(l),
                                                                                         value(v),
                                                                                         func(f)
    {
    }
    void setType(const TokenType &t)
    {
        type = t;
    }
    void setLexme(const std::string &l)
    {
        lexme = l;
    }
    void setValue(const double &v)
    {
        value = v;
    }
    void setFunc(const Func &f)
    {
        func = f;
    }
    TokenType &getType(void)
    {
        return type;
    }
    std::string &getLexme(void)
    {
        return lexme;
    }
    double &getValue(void)
    {
        return value;
    }
    Func &getFunc(void)
    {
        return func;
    }

    friend std::ostream &operator<<(std::ostream &os, const Token &token){//方便打印观察
        os<<token.toString();
        return os;
    }

    /**
     * @brief 转化字符串,方便打印观察
     * 
     * @return std::string 
     */
    std::string toString(void)const{
        return "["+STokenType[type]+","+lexme+","+std::to_string(value)+std::to_string((unsigned long long)func)+"]";
    }
};

/**
 * @brief 内置符号表,使用初始化列表方式构造,需要c++11标准,编译时加入-std=c++11
 * 
 */
static std::vector<Token> build_in_token_table={
    {CONST_ID, "PI", 3.1415926, NULL},
    {CONST_ID, "E", 2.71828, NULL},
    {T, "T", 0.0, NULL},
    {FUNC, "SIN", 0.0, sin},
    {FUNC, "COS", 0.0, cos},
    {FUNC, "TAN", 0.0, tan},
    {FUNC, "LN", 0.0, log},
    {FUNC, "EXP", 0.0, exp},
    {FUNC, "SQRT", 0.0, sqrt},
    {ORIGIN, "ORIGIN", 0.0, NULL},
    {SCALE, "SCALE", 0.0, NULL},
    {ROT, "ROT", 0.0, NULL},
    {IS, "IS", 0.0, NULL},
    {FOR, "FOR", 0.0, NULL},
    {FROM, "FROM", 0.0, NULL},
    {TO, "TO", 0.0, NULL},
    {STEP, "STEP", 0.0, NULL},
    {DRAW, "DRAW", 0.0, NULL},
};

static std::vector<Token*> append_token_table;

