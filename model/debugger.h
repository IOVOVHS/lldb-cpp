#ifndef _DEBUGGER
#define _DEBUGGER

#include "program.h"
#include "command.h"
#include "Variable.h"
#include "proginfo.h"
#include <sqlite3.h>
#include <random>
#include <unordered_map>
#include <vector>
#include "lldb/API/LLDB.h"
/*
    def __init__(self,program:Program) -> None:
        self.cmds:list=[]
        self.vars=[]
        self.rt_info=[]
        self.program=program
        self.target:lldb.SBTarget=program.target
        self.process:lldb.SBProcess=None
        self.lastPos=dict()
        self.breakpoints=[]
  
    def Breakpoint(self,line_no:int):
        """设置断点"""
        self.target.BreakpointCreateByLocation(self.program.file_name,line_no)
        br_cmd=Command("breakpoint",0,str(line_no))
        self.cmds.append(br_cmd)
*/

class Debugger
{
private:
    Program* prog;
    SBTarget* target;
    SBProcess* process;
    std::vector<SBValue*> vars;
    std::unordered_map<Command*,std::vector<Variable*>> rt_info;
    std::vector<SBBreakpoint*> breakpoints;

public:
    Debugger(Program*,SBTarget* target);
    ~Debugger();

    SBProcess* Run();
    void Continue();
    void setBreakpoint(int);
    void setProcess();
    void readChild(SBValue* value,std::vector<Variable*> s_value,std::string value_name);
    void createData(std::vector<Variable*>& vec);
    void storeRTInfo(sqlite3*);
};

#endif