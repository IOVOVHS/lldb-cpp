#include "Trace.h"

// 添加运行路径上的源程序行号
void Trace::insertLine(int line_no, const std::string& function, lldb::SBFileSpec file_spec, bool isSet) {
    if (function == "__libc_start_call_main" || function == "main" || function.empty()) {
        return;
    }

    if (trace.find(function) == trace.end()) {
        trace[function] = std::vector<int>();
        functions.push_back(function);
    }
    if(isSet){
    // 如果该行号已经存在于函数中，则不添加
        if (std::find(trace[function].begin(), trace[function].end(), line_no) != trace[function].end()) {
            return;
        }
    }
    trace[function].push_back(line_no);

    /*
    if(file_func.find(file_spec)==file_func.end()){
        file_func[file_spec]=std::vector<std::string>();
    }
    file_func[file_spec].push_back(function);
    */
    
    std::string file_path = file_spec.GetFilename();
    if(file_func_str.find(file_path)==file_func_str.end()){
        file_func_str[file_path] = std::vector<std::string>();
    }
    file_func_str[file_path].push_back(function);

}

void Trace::countLinesExecuted(int line,std::string& func_name,std::vector<int>& resLines){
    auto lines = trace[func_name];
    auto iter = std::find(lines.rbegin(), lines.rend(), line);
    resLines.insert(resLines.begin(),lines.rbegin(),iter);
}


// 返回路径的字符串表示
std::string Trace::getTraceStr() {
    if (trace_str.empty()) {
        convertToStr();
    }
    return trace_str;
}

// 将路径转换为字符串
void Trace::convertToStr() {
    trace_str = "{";
    for (const auto& func : functions) {
        const std::vector<int>& values = trace[func];
        std::vector<int> v_sorted = values;
        std::sort(v_sorted.begin(), v_sorted.end());

        trace_str += func + ": [";
        for (size_t i = 0; i < v_sorted.size(); ++i) {
            trace_str += std::to_string(v_sorted[i]);
            if (i < v_sorted.size() - 1) {
                trace_str += ", ";
            }
        }
        trace_str += "]; ";
    }
    trace_str += "}";
}


// 展示路径
void Trace::displayTrace() {
    if (trace_str.empty()) {
        convertToStr();
    }
    std::cout << "Trace length: " << trace.size() << std::endl;
    for (const auto& [func, lines] : trace) {
        std::cout << func << ": [";
        for (size_t i = 0; i < lines.size(); ++i) {
            std::cout << lines[i];
            if (i < lines.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
    }
    std::cout << "Trace string: " << trace_str << std::endl;
}


// 返回所有行
std::vector<int>  Trace::getAllLines() const {
    std::vector<int> all_lines;
    for (const auto& [func, lines] : trace) {
        all_lines.insert(all_lines.end(), lines.begin(), lines.end());
    }
    return all_lines;
}


// 返回指定行对应的函数
std::string Trace::getFuncOfLine(int line_f) const {
    for (const auto& [func, lines] : trace) {
        if (std::find(lines.begin(), lines.end(), line_f) != lines.end()) {
            return func;
        }
    }
    return "";
}


std::string Trace::convertToJson(){
    json j_trace = json::array();
    for(auto begin=trace.begin();begin!=trace.end();begin++){
        auto lines=begin->second;
        json j_lines=json::array();
        for(auto line:lines){
            j_lines.push_back(line);
        }
        json j_func={
            {begin->first,j_lines}
        };
        j_trace.push_back(j_func);
    }
    return j_trace.dump();
}

int Trace::getNumOfFunc(){
    return trace.size();
}

int Trace::getLenOfTrace(){
    int len=0;
    for(auto begin=trace.begin();begin!=trace.end();begin++){
        len+=begin->second.size();
    }
    return len;
}

bool Trace::isInclude(int line, std::string func_name){
    auto lines = trace[func_name];
    if(std::find(lines.begin(),lines.end(),line)==lines.end()){
        return true;
    }
    return false;
}

std::vector<std::pair<SBFileSpec, int>> Trace::getLinesForBreak(Trace* debug_trace){
    std::vector<std::pair<SBFileSpec,int>> res;

    for(auto begin = trace.begin(); begin!=trace.end(); begin++){
        for(auto line:begin->second){
            if(!debug_trace->isInclude(line, begin->first)){
                SBFileSpec file=getFileFromFunc(begin->first);
                if(!file){
                    continue;
                    //return std::unordered_map<SBFileSpec*, int>();
                }
                res.push_back({file,line});
            }
        }
    }
    return res;
}

SBFileSpec Trace::getFileFromFunc(std::string func){
    /*
    for(auto begin=file_func.begin();begin!=file_func.end();begin++){
        if(find(begin->second.begin(),begin->second.end(),func)==begin->second.end()){
            return begin->first;
        }
    }
    */

    for(auto iter=file_func_str.begin();iter!=file_func_str.end();iter++){
        if(find(iter->second.begin(),iter->second.end(),func)!=iter->second.end()){
            return SBFileSpec(iter->first.c_str());
        }
    }
    return nullptr;
}