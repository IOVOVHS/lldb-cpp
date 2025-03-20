#ifndef _PROGINFO
#define _PROGINFO

#include <iostream>
#include <vector>
#include <string>
#include <fstream> // 用于文件读取
#include "../test/json.hpp"

using json = nlohmann::json;

class VarInfo {
public:
    std::string name;
    std::string func;
    int decl;


    VarInfo();

    //json数据中的对变量赋值的行
    std::vector<int> assigns;

    //调试时统计的执行到的赋值行,弃用
    std::vector<int> assigns_executed;

    //由变量的声明行和词法作用域得到，在变量合法生命周期的行
    std::vector<int> lines_for_checked;

    //弃用
    void add_assigns_executed(int assign);

    //由运行路径，检查该变量是否已经赋值
    bool isInited(std::vector<int>& vec);

    // 打印信息的辅助函数
    void print() const;
};

class Lexical_Block{

public:
    int level;
    std::vector<Lexical_Block*> lex_blks;
    std::vector<std::pair<int,int>> pos;
    
    //void print() const;

    void add_loc(int line,int column);
};

class Func_Lexical{
public:
    std::string func_name;
    Lexical_Block* lex_blk;
    
    Func_Lexical(std::string func_name, Lexical_Block* lex_b):func_name(func_name),lex_blk(lex_b) {}
    
    //void print();
};


/*
class TraceInfo{
public:
    TraceInfo();

    std::unordered_map<std::string,std::vector<int>> func_lines;

    //找到初始化过的变量,通过判断VarInfo的assigns_executed判断
    void getInitedVarInfo(std::vector<VarInfo*>,int,const std::string&);
};
*/


#endif