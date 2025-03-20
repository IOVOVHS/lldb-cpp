#ifndef _LLDBTEST
#define _LLDBTEST

#include "lldb/API/LLDB.h"
#include "program.h"
#include <string>
#include <fstream>
#include <iostream>
#include "debugger.h"
/*
class LLDBTest:
    def __init__(self,start_func="main",mode="run") -> None:
        self.debugger:lldb.SBDebugger = lldb.SBDebugger.Create()
        self.debugger.SetAsync(False)
        self.start_func = start_func
        self.mode = mode
        #目标程序
        self.target=None
        #可执行文件名
        self.exe=""
        #编译后的执行模块
        self.module=None
        #程序类
        self.program_run=None
*/
using SBDebugger = lldb::SBDebugger;
using SBThread = lldb::SBThread;
using SBFileSpec = lldb::SBFileSpec;

class LLDBTest{
private:
public:
    SBDebugger debugger;
    SBTarget target;
    SBProcess process;
    Program* prog;
    std::string exe;
    void setTarget(std::string& func_name, std::string& var_txt);
    //收集无优化的情况下的运行路径
    void collectTraceLineNoOptimized();
    //收集有有优化情况下的程序路径
    void collectTraceLineOptimized();
    
    //收集程序的执行路径
    void collectTraceLine(bool optimized);
    //重新启动进程
    void restartProcess(Debugger* debug);
    //调用restart启动进程
    void runProcess(sqlite3*,int);
    //调试循环
    void debugLoop();
    //存储程序信息
    void storeProgram();
    
};

#endif