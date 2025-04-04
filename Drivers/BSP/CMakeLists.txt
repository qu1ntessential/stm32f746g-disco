cmake_minimum_required(VERSION 3.15)

# set(BSP_CONF_H_DIR ${CMAKE_SOURCE_DIR}/Src)

file(GLOB_RECURSE SRC_DISCO
        ${CMAKE_CURRENT_LIST_DIR}/STM32746G-Discovery/*.c
)

add_library(bsp STATIC
        Components/ft5336/ft5336.c
        Components/lan8742/lan8742.c

        ${SRC_DISCO}
)

target_include_directories(bsp PUBLIC
        Components/Common
        Components/rk043fn48h
        Components/ft5336
        Components/lan8742
        Components/w25q128j
        STM32746G-Discovery
        # ${BSP_CONF_H_DIR}
)

target_link_libraries(bsp PUBLIC hal)