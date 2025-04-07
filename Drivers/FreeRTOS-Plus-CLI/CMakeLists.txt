cmake_minimum_required(VERSION 3.15)

set(LIBRARY_NAME freertos_cli)

add_library(${LIBRARY_NAME} STATIC
        ${CMAKE_CURRENT_LIST_DIR}/FreeRTOS_CLI.c
)

target_include_directories(${LIBRARY_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/
)

target_compile_definitions(${LIBRARY_NAME} PRIVATE
        configCOMMAND_INT_MAX_OUTPUT_SIZE=1024
)

target_link_libraries(${LIBRARY_NAME} freertos_kernel)
