cmake_minimum_required(VERSION 3.0)
project(CALCULATE_MAP
        VERSION 0.0.1
)

# set(CMAKE_CXX_STANDARD 17)  
# set(CMAKE_CXX_STANDARD_REQUIRED True)



#  ./ 表示的是在当前目录内  ../表示的是在上一级目录下  / 表示的是在子目录内
# ${CMAKE_SOURCE_DIR} 表示当前文件所在目录的绝对地址
include_directories(${CMAKE_SOURCE_DIR}/inc)



# 查找目录下的所有源文件
# 并将名称保存到 SRC_LIST变量
aux_source_directory(./src SRC_LIST)

add_executable(calculate_map main.cpp ${SRC_LIST})
