#include "program.h"

Program::Program(std::string& file_path,std::string& json_path):file_name(file_path)
{
    trace = new Trace();
    debug_trace = new Trace();
    trace_no_op = new Trace();
    readJson(json_path);
}

Program::Program(/* args */)
{
}

Program::~Program()
{
    free(trace);
    free(debug_trace);
    free(trace_no_op);
}



void Program::readJson(std::string& json_path){
    std::ifstream input_file(json_path);
    if (!input_file.is_open()) {
        std::cerr << "Failed to open file!" << std::endl;
        return;
    }

    // 解析 JSON 文件内容
    json j;
    input_file >> j;

    // 关闭文件
    input_file.close();

    // 存储反序列化的 VarInfo 对象
    std::vector<VarInfo> var_infos;
    json var_array=j["var_info"];
    // 遍历 JSON 数组并手动反序列化每个对象
    for (const auto& item : var_array) {
        VarInfo* var_info = new VarInfo();
        var_info->name = item["name"].get<std::string>();        // 手动提取 name
        var_info->func = item["func"].get<std::string>();        // 手动提取 func
        var_info->decl = item["decl"].get<int>();                // 手动提取 decl

        var_info->assigns.clear(); // 清空之前的值
        for (const auto& assign_item : item["assigns"]) {
            var_info->assigns.push_back(assign_item.get<int>()); // 将每个值手动添加到 assigns 中
        }
        vars_info.push_back(var_info); // 将对象添加到容器
    }
}

int Program::storeProgram(sqlite3* db){  
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO programs(file_name,file_path,execution_trace) VALUES(?,?,?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement!" << std::endl;
        return -1;
    }

    // 绑定参数
    sqlite3_bind_text(stmt, 1, file_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, file_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, trace->convertToJson().c_str(), -1, SQLITE_STATIC);
    

    // 执行插入
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert data!" << std::endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    int last_id = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    return last_id; 
}


void Program::addTraceNoOptimized(int line,std::string& str, SBFileSpec file_spec){
    trace_no_op->insertLine(line, str, file_spec, false);
}

void Program::addTraceLine(int line,std::string func_name, SBFileSpec file_spec){
    trace->insertLine(line,func_name, file_spec, true);
}


std::vector<VarInfo*>& Program::getInitedVars(int line, std::string& func_name){
    std::vector<VarInfo*> inited_vars;
    std::vector<int> res;
    trace_no_op->countLinesExecuted(line,func_name,res);
    for(auto var_info:vars_info){
        if(var_info->isInited(res)){
            inited_vars.push_back(var_info);
        }
    }
    return inited_vars;
}

std::vector<int> Program::getTotalLines(){
    return trace->getAllLines();
}

std::vector<int> Program::getTotalDebugLines(){
    return debug_trace->getAllLines();
}

int Program::getLenOfTrace(){
    return trace->getLenOfTrace();
}

int Program::getNumOfFuncitons(){
    return trace->getNumOfFunc();
}

int Program::getLenOfDebugTrace(){
    return debug_trace->getLenOfTrace();
}

int Program::getNumOfDebugFunc(){
    return debug_trace->getNumOfFunc();
}

void Program::addDebugTraceLine(int line, std::string func_name, SBFileSpec file_spec){
    debug_trace->insertLine(line,func_name,file_spec, true);
}

bool Program::isInclude(std::string func_name,int line){
    return trace->isInclude(line, func_name);
}

std::vector<std::pair<SBFileSpec, int>> Program::getLinesForBreak(){
    return trace->getLinesForBreak(debug_trace);
}