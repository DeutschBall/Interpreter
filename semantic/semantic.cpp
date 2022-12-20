#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <iomanip>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "message/message.hpp"

double scale_x;
double scale_y;
double origin_x;
double origin_y;
double rot;

//标记drawer是否被初始化
bool flag_drawer_init=false;
//写文件流
std::ofstream fout;

double getExpValue(Node *root);
void closeDrawer(void);

/**
 * @brief 初始化Drawer,写入tkinter头
 * 
 */
void initDrawer(void){
    fout.open("./drawer.py");
    if(!fout.is_open()){
        error("cannot create or open output file");
    }
    flag_drawer_init=true;
    fout<<"from tkinter import *\n";
    fout<<"drawer=Tk()\n";
    fout<<"drawer.title(\"Dustball's Drawer\")\n";//设置画布标题
    fout<<"canvas = Canvas(drawer,width=1200,height=800)\n";//设置画布规格
    fout<<"canvas.pack()\n";
}

void closeDrawer(void){
    if(fout.is_open()){
        fout<<"input()\n";//等待输入,阻塞程序
        fout.close();
    }
}

/**
 * @brief 画一个像素点
 * 
 * @param x 
 * @param y 
 */
void drawPixel(double x,double y){
    if(!flag_drawer_init){
        initDrawer();
    }
    std::string buffer;
    if(fabs(x)!=NAN&&fabs(x)!=INFINITY&&fabs(y)!=NAN&&fabs(y)!=INFINITY){
        fout<<std::setprecision(6)<<"canvas.create_oval("<<std::setprecision(6)<<x<<","<<std::setprecision(6)<<y<<","<<std::setprecision(6)<<x<<","<<std::setprecision(6)<<y<<")\n";
    }
}

/**
 * @brief 计算以root为根的分析树的算数值
 * 
 * @param root ,分析树根
 * @return double 
 */
double getExpValue(Node *root){
    double result=0;
    switch(root->getType()){
        
        case PLUS:
            result=1.0*getExpValue(root->getLeft())+getExpValue(root->getRight());
            break;
        case MINUS:
            result= 1.0*getExpValue(root->getLeft())-getExpValue(root->getRight());
            break;
        case MUL:
            result= 1.0*getExpValue(root->getLeft())*getExpValue(root->getRight());
            break;
        case DIV:
            result= 1.0*getExpValue(root->getLeft())/getExpValue(root->getRight());
            break;     
        case POWER:
            result= 1.0*pow(getExpValue(root->getLeft()),getExpValue(root->getRight()));
            break;          
        case FUNC:
            result= 1.0*(root->getFunction())(getExpValue(root->getChild()));
            break;
        case T:
            result= 1.0* *(root->getParameter());
            break;
        case CONST_ID:
            result= 1.0*root->getConst();
            break;
        default:
            result= 0.0;
    }
    return result;
}

//计算坐标值,返回值给x,y
static void getCoordinate(Node *exp_x,Node *exp_y,double &x,double &y){

    double coordinate_x=getExpValue(exp_x)*scale_x;//使用scale_x缩放
    double coordinate_y=getExpValue(exp_y)*scale_y;

    double temp=coordinate_x * cos(rot) + coordinate_y *sin(rot);//交换时使用的临时变量
	coordinate_y = coordinate_y * cos(rot) - coordinate_x *sin(rot);//考虑旋转角度,重新换算成标准坐标系
	coordinate_x = temp;
	coordinate_x += origin_x;
	coordinate_y += origin_y;

    x=coordinate_x;
    y=coordinate_y;
}

/**
 * @brief 每次for语句调用一次draw进行画图,绘制隐函数,(X(t),Y(t))
 * 
 * @param pToken ,T类型,使用其value值进行画图
 * @param start ,起始横坐标
 * @param end ,结束横坐标
 * @param step ,步长
 * @param exp_x ,自变量表达式
 * @param exp_y ,因变量表达式
 */
void draw(Token *pToken,double start,double end,double step,Node *exp_x,Node *exp_y){
    double x;
    double y;
    if(!flag_drawer_init){
        initDrawer();
    }
    // for(pToken->value=start;pToken->value<=end;pToken->value+=1.0*step){
    for(pToken->setValue(start);pToken->getValue()<=end;pToken->setValue(1.0*pToken->getValue()+1.0*step)){
        
        getCoordinate(exp_x,exp_y,x,y);
        drawPixel(x,y);
    }
}

