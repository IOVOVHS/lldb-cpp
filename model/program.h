#ifndef _PROGRAM
#define _PROGRAM

#include <string>
#include <iostream>
#include <sqlite3.h>
#include "Trace.h"
#include "proginfo.h"
#include "lldb/API/SBTarget.h"
#include <fstream>

using namespace lldb;

class Program
{
private:
    void readJson(std::string& json_path);

public:
    std::string file_name;
    int prog_id;

    SBProcess* process;

    std::vector<VarInfo*> vars_info;
    
    Trace* trace;
    Trace* debug_trace;
    Trace* trace_no_op;

    Program(std::string&,std::string&);
    Program(/* args */);
    ~Program();
    
    //读取json_info信息

    //存入program
    int storeProgram(sqlite3* db);
    //增加路径
    void addTraceLine(int line,std::string func_name, SBFileSpec file_spec);

    //增加无优化情况下的路径
    void addTraceNoOptimized(int line,std::string& func_name, SBFileSpec file_spec);

    //返回已经初始化的变量
    std::vector<VarInfo*>& getInitedVars(int line,std::string& func_name);

    std::vector<int> getTotalLines();

    std::vector<int> getTotalDebugLines();

    //返回程序的执行路径上的行号数量
    int getLenOfTrace();
    //返回执行路径上函数的数量
    int getNumOfFuncitons();
    //返回值调试过程中路径的行数
    int getLenOfDebugTrace();
    //返回调试过程中函数的数量
    int getNumOfDebugFunc();
    
    void addDebugTraceLine(int line,std::string func_name, SBFileSpec file_spec);

    bool isInclude(std::string func_name, int line);

    std::vector<std::pair<SBFileSpec, int>> getLinesForBreak();
};



#endif