cmake_minimum_required(VERSION 3.20)
project(lab4 C)

set(CMAKE_C_STANDARD 99) # 设置C标准
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror") # 设置编译参数
# message(${CMAKE_BUILD_TYPE})
# if(${CMAKE_BUILD_TYPE} EQUAL "Release")
#     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3") # 设置编译参数
# endif()
LINK_LIBRARIES(m) # 链接math库
add_executable(lab4 lab4.c)