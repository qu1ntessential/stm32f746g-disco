cmake_minimum_required(VERSION 3.15)

set(STARTUP_LOCATION Device/ST/STM32F7xx/Source/Templates/gcc/startup_stm32f746xx.s)
set(STM32_LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F7xx/Source/Templates/gcc/linker/stm32f746nghx_flash.ld)
set(STM32_LINKER_OPTION  )

add_library(cmsis STATIC
        ${STARTUP_LOCATION}
        Device/ST/STM32F7xx/Source/Templates/system_stm32f7xx.c
)

target_include_directories(cmsis PUBLIC
        Core/Include
        Device/ST/STM32F7xx/Include
        # ${CMAKE_SOURCE_DIR}/Drivers/STM32F7xx_HAL_Driver/Inc
        # ${CMAKE_SOURCE_DIR}/Src
)

target_compile_definitions(cmsis PUBLIC
        -DSTM32F746xx
        # -DCORE_CM7
        # -DUSE_HAL_DRIVER
)

target_link_options(cmsis PUBLIC -T${STM32_LINKER_SCRIPT})
