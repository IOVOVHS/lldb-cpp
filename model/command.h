#include <sqlite3.h>
#include <string>
#include <iostream>

using namespace std;

class Command{
public:
    string cmd_type;
    bool is_data;
    string parameter;
    

    Command(const string& cmd_type, int is_data, const string& parameter) 
            : cmd_type(cmd_type), is_data(is_data), parameter(parameter) {}

    //插入命令数据
    void insertCmdSql(sqlite3* con);

    //这里不是用这个函数，好像没用
    //void setIsData();

    //这里也不使用这个函数
    //getEntity()

    int storeCmds(sqlite3* con, int parent_id);

    
};