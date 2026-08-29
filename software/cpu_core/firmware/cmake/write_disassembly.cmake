if(NOT DEFINED OBJDUMP OR NOT DEFINED INPUT OR NOT DEFINED OUTPUT)
    message(FATAL_ERROR "OBJDUMP, INPUT, and OUTPUT must be defined")
endif()

execute_process(
    COMMAND "${OBJDUMP}" -d -S -C -w "${INPUT}"
    OUTPUT_FILE "${OUTPUT}"
    RESULT_VARIABLE disassembly_result
)

if(NOT "${disassembly_result}" STREQUAL "0")
    message(FATAL_ERROR "objdump failed with exit code ${disassembly_result}")
endif()
