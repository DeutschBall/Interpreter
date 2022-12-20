#include <string>
#include <iostream>
#include <fstream>
#include "lexer.hpp"

/// @brief 文件读写
std::ifstream fin;

int line_no;

bool isCapital(const char &c)
{
    return c >= 'A' && c <= 'Z';
}
bool isLower(const char &c)
{
    return c >= 'a' && c <= 'z';
}
bool isAlpha(const char &c)
{
    return isCapital(c) || isLower(c);
}
bool isDigit(const char &c)
{
    return c >= '0' && c <= '9';
}
char toCapital(const char &c)
{
    if (isLower(c))
    {
        return c + 'A' - 'a';
    }
    return c;
}
bool isSpace(const char &c)
{
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f')
    {
        return true;
    }
    return false;
}



/**
 * @brief 给定文本文档目录,打开该文件作为词法分析器输入
 * 
 * @param filepath ,文本文档路径
 * @return 打开成功则返回true 
 * @return 打开失败则返回false 
 */
bool initLexer(std::string filepath)
{
    fin.open(filepath.c_str());
    if (!fin.is_open())
    {
        return false;
    }
    line_no = 1;
    return true;
}

/**
 * @brief 关闭fin文件流
 * 
 */
void closeLexer(void)
{
    if (fin.is_open())
    {
        fin.close();
    }
    line_no = 0;
    for(auto append_token:append_token_table){//清理堆空间
        if(append_token!=NULL){
            delete append_token;
        }
    }
}


/**
 * @brief 从fin文件流中获取一个字符,返回其大写
 * 
 * @return 大写字符,char
 */
static char getChar(void)
{
    char c = fin.get();
    return toCapital(c);
}

/**
 * @brief 最近一次获取的字符在DFA上无法转移,需要退回到fin流中,方便下一次getChar
 * 
 */
static void backChar(const char &c)
{
    fin.putback(c);
}


/**
 * @brief 根据当前字符串查符号表,给符号定性
 * 
 * @param lexme string字符串 
 * @return Token* 
 */
static Token* queryTokenTable(std::string &lexme)
{
    for(int i=0;i<build_in_token_table.size();i++){
        if(build_in_token_table[i].getLexme()==lexme){
            return &build_in_token_table[i];
        }
    }
    // for(auto build_in_table_token:build_in_token_table){
    //     if(build_in_table_token.getLexme()==lexme){
    //         return new Token(build_in_table_token);
    //         // return &build_in_table_token;
    //     }
    // }
    for(int i=0;i<append_token_table.size();i++){
        if(append_token_table[i]->getLexme()==lexme){
            return append_token_table[i];
        }
    }
    return new Token(ERRORTOKEN,"",0.0,NULL);
}

/**
 * @brief 硬编码的DFA
 * 
 * @return Token* 
 */
Token *getToken(void)
{
    Token *token=new Token();//返回值,一个符号
    char character;//当前读取的字符
    std::string buffer="";//字符缓冲区,用于构造lexme

    while (true)//删去空格,换行
    {
        character = getChar();
        if (character == '\n')//如果是回车则行号++
            line_no++;
        if (character == EOF)//文件读取到头了,返回NONTOKEN符号
        {
            token->setType(NONTOKEN);
            return token;
        }
        if (!isSpace(character)) // 
            break;//直到非空格符号才会跳出循环
    }
    //出循环时character已经是非空字符了,直接加入到buffer中,然后根据character在DFA上转移
    buffer+=character;

    //第一个字符是字母或者下划线
    if (isAlpha(character) || character == '_')
    {
        while (true)
        {
            character = getChar();
            if (!isAlpha(character)&&!isDigit(character))
                break;
            buffer+=character;
        }
        backChar(character); //跳出循环时已经多读了下一个token的第一个字符,需要还回fin流中


        token = queryTokenTable(buffer);//查表
        if (token->getType() == ERRORTOKEN)//未识别的符号,认为是自定义变量,添加到附加符号表
        {
            token->setType(T);
            token->setLexme(buffer);
            append_token_table.push_back(token);//压入附加符号表
        }
    }
    // 第一个字符是数字
    else if (isDigit(character))
    {
        while (true)
        {
            character = getChar();
            if (!isDigit(character))
                break;
            buffer+=character;
        }
        if (character == '.')//允许有一个小数点
        {
            buffer+=character;
            while (true)
            {
                character = getChar();
                if (!isDigit(character))
                    break;
                buffer+=character;
            }
        }
        backChar(character); //多读放回
        token->setLexme(buffer);
        token->setType(CONST_ID);
        token->setValue(stod(buffer));//数字直接记录double类型
    }
    
    else
    {
        switch (character)
        {
        case ',':
            token->setType(COMMA);
            break;
        case ';':
            token->setType(SEMICO);
            break;
        case '(':
            token->setType(L_BRACKET);
            break;
        case ')':
            token->setType(R_BRACKET);
            break;
        case '+':
            token->setType(PLUS);
            break;
        case '*'://两种情况,可能转移到幂,可能转移到乘法, 需要再看一个字符
            character = getChar();
            if (character == '*') // 幂类型
            {
                token->setType(POWER);
            }
            else // 乘法类型
            {
                token->setType(MUL);
                backChar(character);
            }
            break;
        case '-'://两种情况,减号或者注释,需要再看一行
            character = getChar();
            if (character == '-') // 注释
            {
                while (character != '\n' && character != EOF)
                    character = getChar();
                backChar(character); 
                token = getToken();
            }
            else // 减号
            {
                token->setType(MINUS);
                backChar(character);
            }
            break;
        case '/'://斜杠开始有两种可能,除法或者注释,需要再看一个字符
            character = getChar();
            if (character == '/') // 下一个字符还是斜杠,注释
            {
                while (character != '\n' && character != EOF)
                    character = getChar();
                backChar(character); 
                token = getToken();
            }
            else// 除法
            {
                token->setType(DIV);
                backChar(character);
            }
            break;
        default: 
            token->setType(ERRORTOKEN);//啥也不认得,实在没法了
            break;
        }
    }
    return token;//统一返回
}