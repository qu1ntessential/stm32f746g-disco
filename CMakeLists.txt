cmake_minimum_required(VERSION 3.15)

set(CMAKE_VERBOSE_MAKEFILE ON)

project(stm32f746g-disco LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(Drivers/CMSIS)
add_subdirectory(Drivers/STM32F7xx_HAL_Driver)
add_subdirectory(Drivers/BSP)
add_subdirectory(Drivers/FatFs)
# add_subdirectory(Drivers/lvgl-9.2.2)
# add_subdirectory(Drivers/STM32_USB_Device_Library)

add_executable(${PROJECT_NAME}
        ${STARTUP_FILE}
)

include(Src/app-sources.cmake)
# include(eez-ui/eez-sources.cmake)

# ---------------------------------------------------------------------------------------------------------------------
set(FREERTOS_KERNEL_PATH "Drivers/FreeRTOS-Kernel-11.1.0")

add_library(freertos_config INTERFACE)

target_include_directories(freertos_config INTERFACE
        "Src/FreeRTOS"
)

if (DEFINED FREERTOS_SMP_EXAMPLE AND FREERTOS_SMP_EXAMPLE STREQUAL "1")
    message(STATUS "Build FreeRTOS SMP example")
    # Adding the following configurations to build SMP template port
    add_compile_options(-DconfigNUMBER_OF_CORES=2 -DconfigUSE_PASSIVE_IDLE_HOOK=0)
endif ()

set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)

set(FREERTOS_PORT "GCC_ARM_CM7" CACHE STRING "" FORCE)

add_subdirectory(${FREERTOS_KERNEL_PATH} FreeRTOS-Kernel)

# ---------------------------------------------------------------------------------------------------------------------

target_link_libraries(${PROJECT_NAME} PUBLIC
        bsp
        freertos_kernel
        fatfs
        lvgl
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Generating HEX and BIN files..."

        # Переименовываем .elf (опционально, если нужно сохранить оригинальный файл)
        COMMAND ${CMAKE_COMMAND} -E rename
        $<TARGET_FILE:${PROJECT_NAME}>
        $<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.elf

        # Конвертируем ELF -> HEX (Intel HEX)
        COMMAND arm-none-eabi-objcopy -O ihex
        $<TARGET_FILE:${PROJECT_NAME}>
        $<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.hex

        # Конвертируем ELF -> BIN (Raw binary)
        COMMAND arm-none-eabi-objcopy -O binary
        $<TARGET_FILE:${PROJECT_NAME}>
        $<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.bin

        # Можно добавить вывод размера файла (опционально)
        COMMAND arm-none-eabi-size $<TARGET_FILE:${PROJECT_NAME}>
)

add_custom_target(erase_flash
        COMMAND ${CMAKE_COMMAND} -E echo "Erasing flash memory..."
        COMMAND openocd -f stm32f746g-disco.cfg -c "init; reset init; stm32f7x mass_erase 0; reset halt; shutdown"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Erasing the flash memory of STM32F746NGH6"
)