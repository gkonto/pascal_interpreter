cmake_minimum_required(VERSION 2.8.7)
PROJECT(interpeter)
add_definitions("-Wall")
cmake_policy(SET CMP0002 OLD)
cmake_policy(SET CMP0000 OLD)


set(HEADERS
   token.hpp
   interpreter.hpp
   )

set(SOURCES 
    token.cpp
    interpreter.cpp
    )

ADD_EXECUTABLE(interpreter ${HEADERS} ${SOURCES})
