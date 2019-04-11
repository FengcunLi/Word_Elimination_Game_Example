cmake_minimum_required(VERSION 3.13)
project(game)

set(CMAKE_CXX_STANDARD 11)
#include_directories(${PROJECT_SOURCE_DIR}/json/include)
add_executable(main_1 single_machine/main_1.cpp core/database.cpp core/utils.cpp)
add_executable(client client_server/client.cpp core/database.cpp core/utils.cpp)
add_executable(server client_server/server.cpp core/database.cpp core/utils.cpp)


#set(SOURCE_FILES main.cpp sample Item.cpp Item.h)
#add_executable(assingment_5 ${SOURCE_FILES})
#target_include_directories(assingment_5 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})