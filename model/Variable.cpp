#include "Variable.h"


// 存储变量
int Variable::storeVar(sqlite3* db, int cmd_id) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO data(data_name, data_type, data_value, cmd_id) VALUES(?,?,?,?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement!" << std::endl;
        return -1;
    }

    // 绑定参数
    sqlite3_bind_text(stmt, 1, data_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, data_type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, data_value.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, cmd_id);

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