# Рекурсивно находим все .cpp и .c файлы в папке Src и её подкаталогах
file(GLOB_RECURSE SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/*.cpp
        ${CMAKE_CURRENT_LIST_DIR}/*.c

        ${CMAKE_SOURCE_DIR}/CoGen/Core/Src/main.c
        ${CMAKE_SOURCE_DIR}/CoGen/Core/Src/stm32f7xx_it.c
        ${CMAKE_SOURCE_DIR}/CoGen/Core/Src/dma.c
        ${CMAKE_SOURCE_DIR}/CoGen/Core/Src/dac.c
        ${CMAKE_SOURCE_DIR}/CoGen/Core/Src/usart.c
        ${CMAKE_SOURCE_DIR}/CoGen/Core/Src/i2c.c
        ${CMAKE_SOURCE_DIR}/CoGen/Core/Src/tim.c
        ${CMAKE_SOURCE_DIR}/CoGen/Core/Src/ltdc.c
        ${CMAKE_SOURCE_DIR}/CoGen/Core/Src/iwdg.c
        ${CMAKE_SOURCE_DIR}/CoGen/Core/Src/stm32f7xx_hal_msp.c
        ${CMAKE_SOURCE_DIR}/CoGen/Core/Src/stm32f7xx_hal_timebase_tim.c
)

# Исключаем указанные файлы (все файлы, расположенные на два уровня ниже Legacy)
file(GLOB_RECURSE EXCLUDE_FILES
        ${CMAKE_CURRENT_LIST_DIR}/Legacy/*/*.*
)

foreach (file ${EXCLUDE_FILES})
    list(REMOVE_ITEM SOURCES ${file})
endforeach ()

# Добавляем найденные исходники в target_sources
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${SOURCES})

# Путь к папке Src
set(SRC_DIR ${CMAKE_CURRENT_LIST_DIR})

# Рекурсивно находим все директории в папке Src и её подкаталогах
file(GLOB_RECURSE ALL_DIRS LIST_DIRECTORIES TRUE ${SRC_DIR}/*)

# Добавляем только директории
set(INCLUDE_DIRS "")
foreach (item ${ALL_DIRS})
    if (IS_DIRECTORY ${item})
        list(APPEND INCLUDE_DIRS ${item})
    endif ()
endforeach ()

# Убираем директории внутри Src/Legacy из списка include-путей
set(FILTERED_INCLUDE_DIRS "")
foreach (dir ${INCLUDE_DIRS})
    string(FIND "${dir}" "${CMAKE_CURRENT_LIST_DIR}/Legacy" _pos)
    if(_pos EQUAL -1)
        list(APPEND FILTERED_INCLUDE_DIRS ${dir})
    endif()
endforeach()

# Добавляем папки в target_include_directories (без Legacy)
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        ${FILTERED_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/CoGen/Core/Inc/
)
