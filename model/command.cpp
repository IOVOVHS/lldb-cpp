#include "command.h"


void Command::insertCmdSql(sqlite3* con) {
    // 插入命令数据
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO command(cmd_type,is_data,parameter) VALUES(?,?,?)";

    if (sqlite3_prepare_v2(con, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Failed to prepare statement!" << endl;
        return;
    }

    // 绑定参数
    sqlite3_bind_text(stmt, 1, cmd_type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, is_data);
    sqlite3_bind_text(stmt, 3, parameter.c_str(), -1, SQLITE_STATIC);

    // 执行插入
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert data!" << std::endl;
    }

    sqlite3_finalize(stmt);
}




int Command::storeCmds(sqlite3* con, int parent_id) {
    // 存储命令
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO command(cmd_type,is_data,parameter,program_id) VALUES(?,?,?,?)";

    if (sqlite3_prepare_v2(con, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement!" << std::endl;
        return -1;
    }

    // 绑定参数
    sqlite3_bind_text(stmt, 1, cmd_type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, is_data);
    sqlite3_bind_text(stmt, 3, parameter.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, parent_id);

    // 执行插入
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert data!" << std::endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    int last_id = sqlite3_last_insert_rowid(con);
    sqlite3_finalize(stmt);
    return last_id;
}