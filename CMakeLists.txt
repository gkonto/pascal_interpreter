cmake_minimum_required(VERSION 2.8.7)
PROJECT(interpeter)
add_definitions("-Wall -std=c++0x")
cmake_policy(SET CMP0002 OLD)
cmake_policy(SET CMP0000 OLD)


set(HEADERS
   clog.hpp
   token.hpp
   interpreter.hpp
   )

set(SOURCES 
    clog.cpp
    token.cpp
    interpreter.cpp
    )

ADD_EXECUTABLE(interpreter ${HEADERS} ${SOURCES})
