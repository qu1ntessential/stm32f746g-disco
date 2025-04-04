cmake_minimum_required(VERSION 3.15)

set(LIBRARY_NAME eez-ui)

# Найти все C-файлы в директории
file(GLOB_RECURSE ALL_C_SOURCES
        src/*.c
)

# Исключить некоторые файлы
list(REMOVE_ITEM ALL_C_SOURCES
        # ${CMAKE_CURRENT_SOURCE_DIR}/src/ui.c
)

add_library(${LIBRARY_NAME} STATIC
        ${ALL_C_SOURCES}
)

target_include_directories(${LIBRARY_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
        # ${CMAKE_CURRENT_SOURCE_DIR}/src/ui/images
)

target_link_libraries(${LIBRARY_NAME} PUBLIC bsp lvgl freertos_kernel)
