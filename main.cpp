#include <iostream>
#include <cstring>
#include "message/message.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "semantic/semantic.hpp"

using namespace std;


int main(int argc,char **argv){
    char choice;
    if(argc<2){
        error("Usage: ./Dustcomplier <source file>");
    }
    if(!initLexer(argv[1])){
        error("failed to open source file "+string(argv[1]));
    }
    if(argv[2]&&!strcmp(argv[2],"-token")){//只看所有的符号,检查词法分析器的工作
        Token *token;
        while(true){
            token=getToken();
            if(token->getType()==NONTOKEN){
                break;
            }
            else{
                cout<<*token<<endl;
            }
        }
        exit(0);
    }
    cout<<"parser activing..."<<endl;//提示开始语法分析
    parser();
    cout<<"parser actived"<<endl;//提示语法分析结束
    closeLexer();
    closeDrawer();
    return 0;
}


