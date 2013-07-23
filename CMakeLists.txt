cmake_minimum_required(VERSION 2.6)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(CppArgParser)

add_executable(ArgParserTest ArgParserTest.cpp ArgParser.cpp)
TARGET_LINK_LIBRARIES(ArgParserTest boost_system boost_program_options)

enable_testing()

ADD_DEFINITIONS("-std=c++0x") 
