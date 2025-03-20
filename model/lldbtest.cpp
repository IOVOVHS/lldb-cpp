#include "lldbtest.h"

/*
    def setTarget(self,exe:str,breakpoint_nums:int,var_txt:str) -> None:
        """设置用于调试的目标程序"""
        self.exe=exe
        if not os.path.exists(exe):
            logging.warning(f"file {exe} not exists")
            return
        self.target:lldb.SBTarget=self.debugger.CreateTarget(exe)
        exe_path=os.path.realpath(exe)
        modules=list(filter(lambda m: m.file.fullpath == exe_path, self.target.modules))
*/


LLDBTest::LLDBTest(){}

LLDBTest::~LLDBTest(){
    free(prog);
}

void LLDBTest::setTarget(std::string& exe, std::string& json_path){
    this->exe = exe;

    std::fstream f(exe);
    if(!f.good()){
        std::cout << "file is not exists" << std::endl;
        return;
    }
    debugger = SBDebugger::Create();
    debugger.SetAsync(false);
    target = debugger.CreateTarget(exe.c_str());
    prog = new Program(exe,json_path);
}

void LLDBTest::collectTraceLineNoOptimized(){
    collectTraceLine(false);
}

void LLDBTest::collectTraceLineOptimized(){
    collectTraceLine(true);
}

void LLDBTest::collectTraceLine(bool optimized){
    process = target.LaunchSimple(NULL,NULL,".");
    if(process.IsValid()){
        SBThread thread = process.GetSelectedThread();
        auto state = process.GetState();
        if(state!=eStateStopped){
            std::cout << "process didn't stopped" << std::endl;
        }
    }
    while(true){
        if(process.GetState()==eStateExited){
            return;
        }
        auto thread = process.GetThreadAtIndex(0);
        auto frame = thread.GetFrameAtIndex(0);
        if(!frame && !frame.GetFunctionName()){
            std::cout<< "empty frame" <<std::endl;
            return;
        }

        auto line = frame.GetLineEntry().GetLine();
        std::string func_name = frame.GetFunctionName();
        auto file_spec = frame.GetModule().GetFileSpec();
        
        //这里因为file_spec内有unique_ptr，所以不直接使用，构建一个新对象
        //SBFileSpec file = new SBFileSpec(file_spec);
        //运算符重载内部实现了unique_str的深复制
        SBFileSpec file = file_spec;

        if(optimized){
            prog->addTraceLine(line,func_name,file);
        }else{
            prog->addTraceNoOptimized(line,func_name,file);
        }
        //在csmith的platform_xxx函数中,库函数和rand()函数跳出去，因为这两个都没法调试
        //这里不再使得只有func开头的函数才能被调试
        if(func_name.find("platform_main") || func_name.find("lib") || func_name=="rand" ){
            thread.StepOut();
        }
        thread.StepInto();
    }
}

void LLDBTest::restartProcess(Debugger* debugger){
    auto process = debugger->Run();
    if(!process){
        std::cout<< "process didn't exits" << std::endl;
        return;
    }
    if(process->GetState() != eStateStopped){
        std::cout << "process didn't stopped" << std::endl;
    }

    while(true){
        if(process->GetState()==eStateExited){
            return;
        }
        debugger->Continue();
    }
}

void LLDBTest::runProcess(sqlite3* con,int break_num){
    process = target.GetProcess();
    if(process.GetState() != eStateExited){
        std::cout << "process didn't stop" << std::endl;
    }
    target.DeleteAllBreakpoints();
    Debugger* debug_lldb =  new Debugger(prog,&target);
    debug_lldb->setBreakpoint(break_num);
    restartProcess(debug_lldb);
    debug_lldb->storeRTInfo(con);
}

void LLDBTest::debugLoop(){
    int trace_len = prog->getLenOfTrace();
    int function_num = prog->getNumOfFuncitons();

    sqlite3* db;
    char* errorMessage = 0;
    
    // 打开数据库（如果不存在则创建）
    int exit = sqlite3_open("lldb_var.db", &db);
    
    if (exit) {
        std::cerr << "无法打开数据库: " << sqlite3_errmsg(db) << std::endl;
        return;
    } else {
        std::cout << "数据库成功打开!" << std::endl;
    }

    while(true){
        int debug_trace_len = prog->getLenOfDebugTrace();
        std::cout << "debug trace len : " << debug_trace_len << "\n";
        int debug_func_num = prog->getNumOfFuncitons();
        std::cout << "debug func nums : " << debug_func_num << "\n";
        if(function_num <=  debug_func_num && debug_trace_len>=trace_len/5){
            return;
        }else{
            runProcess(db,5);
        }
    }
}

void LLDBTest::storeProgram(){
    sqlite3* db;
    char* errMsg = 0;

    // 打开数据库（如果不存在则创建数据库文件）
    int rc = sqlite3_open("example.db", &db);

    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    } else {
        std::cout << "Opened database successfully" << std::endl;
    }
    prog->storeProgram(db);
}

