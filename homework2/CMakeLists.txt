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
# Define a compiler flag to control logging
option(ENABLE_LOGGING "Enable logging" OFF)
add_executable(dpgen ${SOURCES})
# If logging is enabled, add the corresponding compiler flag
if(ENABLE_LOGGING)
    target_compile_definitions(dpgen PRIVATE ENABLE_LOGGING)
endif()
