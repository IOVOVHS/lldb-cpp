#include "debugger.h"

Debugger::Debugger(Program* prog,SBTarget* target):prog(prog)
{
    target=target;
}

Debugger::~Debugger()
{
}

SBProcess* Debugger::Run(){
    if (target->IsValid()) {
        process = new SBProcess(target->LaunchSimple(NULL, NULL, "."));
        if (!process->IsValid()) {
            return nullptr;
        }
        Command* cmd = new Command("run",1,"null");
        rt_info[cmd]=std::vector<Variable*>();
        if(process->GetState() == eStateStopped){
            createData(rt_info[cmd]);
        }
    }
    return process;
}

void Debugger::Continue(){
    process->Continue();
    Command* cmd = new Command("continue",1,"null");
    rt_info[cmd]=std::vector<Variable*>();
    if(process->GetState() == eStateStopped){
        createData(rt_info[cmd]);
    }
}

void Debugger::setBreakpoint(int break_num){
    //auto lines_collected = prog->getLenOfTrace();
    //auto debug_lines = prog->getLenOfDebugTrace();
    
    std::vector<std::pair<SBFileSpec, int>> pos_collected = prog->getLinesForBreak();
    
    std::vector<int> random_sites;
    int p=0;
    while(p<break_num && p<pos_collected.size()){
        auto random_num = std::rand() % (int)pos_collected.size();
        if(find(random_sites.begin(),random_sites.end(),random_num)==random_sites.end()){
            random_sites.push_back(random_num);
        }
        p++;
    }
    //找到文件和行之后，断点
    for(auto random_site:random_sites){
        auto pos =  pos_collected[random_site];
        target->BreakpointCreateByLocation(pos.first,pos.second);
    }
    //
}

/*
    def createData(self):
        """收集当前位置的数据"""
        thread:lldb.SBThread=self.process.GetThreadAtIndex(0)
        frame:lldb.SBFrame=thread.GetFrameAtIndex(0)
        s_value=[]
        func_args=frame.args
        func_locals=frame.locals
        module:lldb.SBModule=frame.GetModule()
        lineEntry:lldb.SBLineEntry=frame.line_entry
        line=lineEntry.line
        init_value=self.program.alive_vars.GivenVar(line)
*/

void Debugger::createData(std::vector<Variable*>& s_vars){
    auto thread = process->GetThreadAtIndex(0);
    auto frame = thread.GetFrameAtIndex(0);
    auto func = frame.GetFunction();
    //std::vector<Variable*> s_vars;
    
    auto func_args = frame.GetVariables(true,false,false,false);
    auto func_locals = frame.GetVariables(false,true,false,false);
    auto statics = frame.GetVariables(false,false,true,false);
    auto line = frame.GetLineEntry().GetLine();
    std::string func_name = frame.GetFunctionName();
    
    //原先这里有一步是为了判断断点是否陷入了循环，陷入了的话，就会删除这个断点。
    //现在不用，如果超时，直接终止进程，开始下一个程序
    for(int i=0;i<func_args.GetSize();i++){
        auto arg = func_args.GetValueAtIndex(i);
        auto name = arg.GetName();
        readChild(&arg, s_vars, name);
    }

    //统计在当前行时，已经执行到的行。
    //1. 修改执行路径添加行的方式
    //2. 在varinfo类，添加传入参数为当前行，返回bool类型的函数，判断是否初始化

    auto res =  prog->getInitedVars(line, func_name);

    for(int i=0;i<func_locals.GetSize();i++){
        auto var = func_locals.GetValueAtIndex(i);
        for(auto varinfo:res){
            if(varinfo->name == var.GetName()){
                readChild(&var, s_vars, var.GetName());
            }
        }
    }

    for(int i=0;i<statics.GetSize();i++){
        SBValue global_var = statics.GetValueAtIndex(i);
        auto name = global_var.GetName();
        readChild(&global_var, s_vars, name);
    }
}


/*
    def readChild(self,var:lldb.SBValue,s_value:list,value_name:str):
        if var.value=="0x0000000000000000":
            return
        if var.num_children ==0 :
            type=var.GetTypeName()
            value=var.GetValue()
            name=var.GetName()
            variable=Variable(var.path,type,value,value_name)
            s_value.append(variable)
        else:
            for child in var.children:
                child:lldb.value
                self.readChild(child,s_value,value_name)
        liveVars=self.program.alive_vars
*/
void Debugger::readChild(SBValue* var,std::vector<Variable*> s_value,std::string var_name){
    //即 void*0  ,空指针
    if(var->GetValue()=="0x0000000000000000"){
        return;
    }
    int num_child = var->GetNumChildren();
    //没有孩子节点，就可以存储变量了
    if(num_child==0){
        auto type = var->GetTypeName();
        auto value = var->GetValue();
        auto name = var->GetName();
        lldb::SBStream stream;
        auto isPath = var->GetExpressionPath(stream);
        std::string path="";
        if(isPath){
            path = stream.GetData();
        }
        Variable* variable = new Variable(path,type,value,var_name);
        s_value.push_back(variable);
    }else{
        auto n = var->GetChildAtIndex(0);
        for(int i=0;i<num_child;i++){
            auto child = var->GetChildAtIndex(i);
            readChild(&child,s_value,var_name);
        }
    }
}
/*
    def storeRTInfo(self,conn:sqlite3.Connection):
        """调试结束后，存储一次调试过程中运行时信息"""
        cursor=conn.cursor()
        cmd_id=-1
        for info in self.rt_info:
            if isinstance(info,Command):
                cmd_id=info.storeCmds(cursor,self.program.program_id)
            if isinstance(info,list):
                for v in info:
                    if isinstance(v,Variable):
                        v.storeVar(cursor,cmd_id)
                    #递归结构的变量
                    if isinstance(v,TreeNode):
                        v.storeTree(cursor)
        cursor.close()
        conn.commit()
*/

void Debugger::storeRTInfo(sqlite3* db){
    sqlite3_stmt* stmt;
    
    int cmd_id = -1;
    for(auto iter=rt_info.begin();iter!=rt_info.end();iter++){
        cmd_id = iter->first->storeCmds(db,prog->prog_id);
        for(auto var:iter->second){
            var->storeVar(db,cmd_id);
        }
    }
}