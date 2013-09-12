cmake_minimum_required(VERSION 2.6)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(CppArgParser)

add_executable(ParserTest ParserTest.cpp)
add_executable(AppNameTest AppNameTest.cpp)
#TARGET_LINK_LIBRARIES(ParserTest)

ADD_DEFINITIONS("-std=c++0x")
ADD_DEFINITIONS("-g")



#ADD_DEFINITIONS("-fcolor_diagnostics")
#TARGET_LINK_LIBRARIES(ArgParserTest boost_system boost_program_options)
