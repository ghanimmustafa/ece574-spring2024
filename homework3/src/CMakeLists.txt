set(SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(SOURCES 
    ${SRCDIR}/main.cpp
    ${SRCDIR}/NetlistParser.cpp
)

# Define a compiler flag to control logging
#option(ENABLE_LOGGING "Enable logging" OFF)
add_executable(hlsyn ${SOURCES})
# If logging is enabled, add the corresponding compiler flag
if(ENABLE_LOGGING)
    target_compile_definitions(dpgen PRIVATE ENABLE_LOGGING)
endif()
