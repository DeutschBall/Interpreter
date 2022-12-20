#pragma once
#include "parser/parser.hpp"
extern double scale_x;
extern double scale_y;
extern double origin_x;
extern double origin_y;
extern double rot;

extern double getExpValue(Node *root);
extern void draw(Token *pToken,double start,double end,double step,Node *exp_x,Node *exp_y);
extern void closeDrawer(void);