cmake_minimum_required(VERSION 3.10)
project(VerilogGenerator)

set(CMAKE_CXX_STANDARD 17)
set(SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")
set(SOURCES 
    ${SRCDIR}/main.cpp
    ${SRCDIR}/NetlistParser.cpp
    ${SRCDIR}/VerilogGenerator.cpp
    ${SRCDIR}/graph.cpp

)

add_executable(dpgen ${SOURCES})