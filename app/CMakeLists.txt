cmake_minimum_required(VERSION 3.10...3.20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/app)

set(HEADERS
    cementplantmainwindow.h
)

set(FORMS
    cement_plant_window.ui
)

set(SOURCES
    cementplantmainwindow.cpp
    main.cpp
)

add_executable(app_${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS})

if(UNIX)
    target_link_libraries(app_${PROJECT_NAME} ${DL_LIBRARY} )
elseif(WIN32)
    target_link_libraries(app_${PROJECT_NAME} kernel32 )
endif()

target_link_libraries(app_${PROJECT_NAME} ${PROJECT_NAME} gui_${PROJECT_NAME})
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build detected, adding DEBUG_MODE definition")
    target_compile_definitions(${PROJECT_NAME} PRIVATE CementPlantV1DEBUG)
endif()

target_include_directories( app_${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src )
target_include_directories(app_${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
)

qt_standard_project_setup()
