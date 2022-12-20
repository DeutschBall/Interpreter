#include <iostream>
#include <string>
/**
 * @brief 打印失败消息然后终止程序
 * 
 * @param msg ,需要打印的消息
 */
void error(const std::string &msg)
{
    std::cerr << "fatal error:" << msg <<std::endl;
    std::cerr << "compilation aborted" << std::endl;
    exit(-1);
}