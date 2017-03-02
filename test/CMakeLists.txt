
include_directories (${CMAKE_SOURCE_DIR}/test
                     ${CMAKE_SOURCE_DIR}/src
                    )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/test/bin )

add_executable (IniProcessingValidator
                validate.cpp
                ../src/ini_processing.cpp
                )

set_target_properties(IniProcessingValidator PROPERTIES COMPILE_DEFINITIONS "GSL_THROW_ON_CONTRACT_VIOLATION")


add_test (NAME IniProcessingValidator WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test/bin COMMAND IniProcessingValidator)

