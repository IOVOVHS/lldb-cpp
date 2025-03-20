cmake_minimum_required(VERSION 3.10)

# 项目信息
project(lldb_example)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


include_directories(SYSTEM "/opt/homebrew/Cellar/llvm/18.1.8/include")
link_libraries("/opt/homebrew/Cellar/llvm/18.1.8/lib/liblldb.dylib"  )


# 添加可执行文件
add_executable(lldb_example test_API_C++.cpp)

# 链接 LLDB 库
target_link_libraries(lldb_example PRIVATE /opt/homebrew/Cellar/llvm/18.1.8/lib)

# 如果你的系统或安装方法不支持 find_package 自动查找 LLDB
# 可以使用以下手动配置方式
# include_directories(/usr/include/lldb)  # 替换为你系统中的头文件路径
# target_link_libraries(lldb_example /usr/lib/liblldb.so)  # 替换为你系统中的 LLDB 库路径


#target_link_libraries(lldb_example
#  "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>")