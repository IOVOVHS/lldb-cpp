#ifndef _VARIABLE
#define _VARIABLE

#include <iostream>
#include <string>
#include <tuple>
#include <sqlite3.h>

class Variable {
private:
    std::string data_name;
    std::string data_type;
    std::string data_value;
    std::string value_name;

public:
    Variable(){}
    // 构造函数
    Variable(const std::string& data_name, const std::string& data_type, const std::string& data_value, const std::string& value_name) 
        : data_name(data_name), data_type(data_type), data_value(data_value), value_name(value_name) {}

    int storeVar(sqlite3* db, int cmd_id);

};

#endif