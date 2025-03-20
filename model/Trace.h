#ifndef _TRACE
#define _TRACE


#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>
#include "../test/json.hpp"
#include "lldb/API/LLDB.h"

using json = nlohmann::json;

using namespace lldb;

class Trace {
private:
    //std::unordered_map<SBFileSpec, std::vector<std::string>> file_func;
    std::unordered_map<std::string, std::vector<std::string>> file_func_str;
    std::string trace_str;
    std::vector<std::string> functions;

public:
    std::unordered_map<std::string, std::vector<int>> trace;
    Trace() : trace_str("") {}

    // 添加运行路径上的源程序行号
    void insertLine(int line_no, const std::string& function, lldb::SBFileSpec file_spec,bool isSet);

    // 返回路径的字符串表示
    std::string getTraceStr();

    //统计在断点行前，有哪些行是已经运行到的
    void countLinesExecuted(int line,std::string& func_name, std::vector<int>& resLines);

    // 将路径转换为字符串
    void convertToStr();

    // 将路径转换为json字符串
    std::string convertToJson();

    // 展示路径
    void displayTrace();

    // 返回所有行
    std::vector<int> getAllLines() const;

    // 返回指定行对应的函数
    std::string getFuncOfLine(int line_f) const;

    //返回函数的个数
    int getNumOfFunc();

    //返回路径的数量
    int getLenOfTrace();

    //是否包含
    bool isInclude(int line, std::string func_name);

    std::vector<std::pair<SBFileSpec, int>> getLinesForBreak(Trace*);

    SBFileSpec getFileFromFunc(std::string Func);
};


#endif