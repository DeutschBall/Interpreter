cmake_minimum_required(VERSION 3.00)
project(DustCompilier)
set(CMAKE_CXX_STANDARD 11)
AUX_SOURCE_DIRECTORY(. SRC_DIRS)
include_directories("${PROJECT_SOURCE_DIR}")

add_subdirectory(message)
add_subdirectory(parser)
add_subdirectory(lexer)
add_subdirectory(semantic)

add_executable(DustComp main.cpp)
target_link_libraries(DustComp libmessage libparser liblexer libsemantic)
