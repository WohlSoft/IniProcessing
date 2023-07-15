
include_directories (${CMAKE_SOURCE_DIR}/test
                     ${CMAKE_SOURCE_DIR}/src
                    )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/bin" )

add_executable (IniProcessingValidator
                validate.cpp
                ../src/ini_processing.cpp
                )

set_target_properties(IniProcessingValidator PROPERTIES COMPILE_DEFINITIONS "GSL_THROW_ON_CONTRACT_VIOLATION")
add_test (NAME IniProcessingValidator WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}" COMMAND IniProcessingValidator)

if(WIN32)
    add_executable (IniProcessingValidatorU16
                    validate-u16.cpp
                    ../src/ini_processing.cpp
                    )

    set_target_properties(IniProcessingValidatorU16 PROPERTIES COMPILE_DEFINITIONS "GSL_THROW_ON_CONTRACT_VIOLATION")
    add_test (NAME IniProcessingValidatorU16 WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}" COMMAND IniProcessingValidatorU16)
endif()
