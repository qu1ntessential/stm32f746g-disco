# Рекурсивно находим все .cpp и .c файлы в папке Src и её подкаталогах
file(GLOB_RECURSE SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/CoGen/Core/Src/main.c
        ${CMAKE_CURRENT_LIST_DIR}/CoGen/Core/Src/stm32f7xx_it.c
        ${CMAKE_CURRENT_LIST_DIR}/CoGen/Core/Src/usart.c
        ${CMAKE_CURRENT_LIST_DIR}/CoGen/Core/Src/i2c.c
        ${CMAKE_CURRENT_LIST_DIR}/CoGen/Core/Src/tim.c
        ${CMAKE_CURRENT_LIST_DIR}/CoGen/Core/Src/ltdc.c
        ${CMAKE_CURRENT_LIST_DIR}/CoGen/Core/Src/dma2d.c
        ${CMAKE_CURRENT_LIST_DIR}/CoGen/Core/Src/stm32f7xx_hal_msp.c
        ${CMAKE_CURRENT_LIST_DIR}/CoGen/Core/Src/stm32f7xx_hal_timebase_tim.c
        ${CMAKE_CURRENT_LIST_DIR}/CoGen/Core/Src/syscalls.c
        ${CMAKE_CURRENT_LIST_DIR}/CoGen/Core/Src/sysmem.c
)

# Исключаем указанные файлы
set(EXCLUDE_FILES
        # ${CMAKE_CURRENT_LIST_DIR}/example.c
)

foreach (file ${EXCLUDE_FILES})
    list(REMOVE_ITEM SOURCES ${file})
endforeach ()

# Добавляем найденные исходники в target_sources
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${SOURCES})

# Путь к папке Src
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/Src)

# Рекурсивно находим все директории в папке Src и её подкаталогах
file(GLOB_RECURSE ALL_DIRS LIST_DIRECTORIES TRUE ${SRC_DIR}/*)

# Добавляем только директории
set(INCLUDE_DIRS "")
foreach (item ${ALL_DIRS})
    # Получаем директорию файла
    get_filename_component(dir_path "${item}" DIRECTORY)

    # Добавляем директорию, если её ещё нет в списке
    list(FIND INCLUDE_DIRS "${dir_path}" found)
    if (found EQUAL -1)
        list(APPEND INCLUDE_DIRS "${dir_path}")
    endif ()
endforeach ()

message(STATUS "INCLUDE_DIRS: ${INCLUDE_DIRS}")

# Добавляем папки в target_include_directories
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        ${INCLUDE_DIRS}
)
