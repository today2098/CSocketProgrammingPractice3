cmake_minimum_required(VERSION 3.16)
project(CSocketProgrammingPractice C)

# 実行ファイルの作成場所を指定．
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

message("[debug] CMAKE_C_COMPILER:               ${CMAKE_C_COMPILER}")
message("[debug] CMAKE_SOURCE_DIR:               ${CMAKE_SOURCE_DIR}")
message("[debug] PROJECT_SOURCE_DIR:             ${PROJECT_SOURCE_DIR}")
message("[debug] CMAKE_CURRENT_BINARY_DIR:       ${CMAKE_CURRENT_BINARY_DIR}")
message("[debug] CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

add_subdirectory(lib)
add_subdirectory(src)
