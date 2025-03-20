#include <string>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include "lldbtest.h"

typedef lldb::pid_t pid_t_sub;

namespace fs = std::filesystem;

void runDebugger(std::string& exe_file,std::string& var_init_path){
    std::cout<< exe_file <<"\n";
    auto lldb_test = new LLDBTest();
    lldb_test->setTarget(exe_file,var_init_path);
    lldb_test->collectTraceLineOptimized();
    lldb_test->collectTraceLineOptimized();
    lldb_test->storeProgram();
    lldb_test->debugLoop();
    lldb_test->debugger.Clear();
    SBDebugger::Destroy(lldb_test->debugger);
}

void timeout_handler(int sig) {
    std::cerr << "Process timed out!" << std::endl;
}

std::vector<std::string> getFilesFromPath(std::string& path){
   std::vector<std::string> resFiles;

    if (!fs::exists(path) || !fs::is_directory(path)) {
        std::cerr << "The file path is not a valid directory!" << std::endl;
        return resFiles;
    }

    // 遍历目录及其子目录中的所有文件
    try {
        for (const auto& entry : fs::recursive_directory_iterator(path)) {
            
            if (fs::is_regular_file(entry.path())) {
                entry.path().filename().c_str();
                //std::cout << fs::absolute(entry.path()) << std::endl;
                resFiles.push_back(fs::absolute(entry.path().filename().c_str()));
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error accessing directory: " << e.what() << std::endl;
        return resFiles;
    }
    return resFiles;
}


int main(int argc,char *argv[]) {
    if(argc!=3){
        std::cout << "argments'num is not right" << std::endl;
    }

    std::string exe_path = argv[1];
    std::string var_info_path = argv[2];

    //设置超时时间为10s
    int timeout_seconds = 10;

    std::vector<std::string> exe_files = getFilesFromPath(exe_path);
    std::vector<std::string> var_info_files = getFilesFromPath(var_info_path);
    
    for(auto exe_file:exe_files){
        if(find(var_info_files.begin(),var_info_files.end(),exe_file)==var_info_files.end()){
            continue;
        }
        auto exe = exe_path+exe_file;
        auto var_info = var_info_path + exe_file + ".txt";

        
        lldb::pid_t pid = fork();

        if (pid < 0) {
            std::cerr << "Fork failed!" << std::endl;
            return 1;
        } else if (pid == 0) {
            // 子进程执行函数
            runDebugger(exe, var_info);
        } else {
            // 父进程，等待子进程
            signal(SIGALRM, timeout_handler); // 设置超时处理函数
            alarm(timeout_seconds);           // 设置超时信号

            int status;
            lldb::pid_t result = waitpid(pid, &status, WNOHANG); // 等待子进程结束

            int elapsed_time = 0;
            while (result == 0 && elapsed_time < timeout_seconds) {
                sleep(1); // 等待 1 秒
                elapsed_time++;
                result = waitpid(pid, &status, WNOHANG); // 检查子进程是否结束
            }

            if (result == 0) {
                 std::cout << "Process " << exe << " terminated." << std::endl;
                kill(pid, SIGKILL); // 发送 SIGKILL 信号终止子进程
                waitpid(pid, &status, 0); // 确保子进程终止
                std::cout << "Process " << pid << ":" << exe << " was terminated." << std::endl;
            } else if (WIFEXITED(status)) {
                std::cout << "Process " << pid << ":" << exe << " completed successfully." << std::endl;
            } 

            // 取消定时器
            alarm(0);
        }
    }
    std::cout << "All processes finished." << std::endl;

    return 0;
}