# message("Path to IniProcessor is [${CMAKE_CURRENT_LIST_DIR}]")
include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

set(INIPROCESSOR_SRCS)

list(APPEND INIPROCESSOR_SRCS
    ${CMAKE_CURRENT_LIST_DIR}/include/IniProcessor/ini_processing.h
    ${CMAKE_CURRENT_LIST_DIR}/include/IniProcessor/ini_processing_variant.h
    ${CMAKE_CURRENT_LIST_DIR}/src/ini_processing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ini_processing_variant.cpp
)
