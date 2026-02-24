cmake_minimum_required(VERSION 3.10...3.20)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/src)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

qt_standard_project_setup()

set(HEADERS
    cement_plant.h
    gui/classroomwidget.h
)

set(FORMS
    gui/class_room.ui
)

set(SOURCES
    cement_plant.cpp
    gui/classroomwidget.cpp
)

if (BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES} ${FORMS})
else ()
    add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} ${FORMS})
endif()
add_library(handlers::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC Qt6::Core Qt6::Widgets)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build detected, adding DEBUG_MODE definition")
    target_compile_definitions(${PROJECT_NAME} PRIVATE CementPlantV1DEBUG)
endif()

target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} )
target_include_directories( ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/gui )
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_include_directories(${PROJECT_NAME} PUBLIC
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Widgets_INCLUDE_DIRS}
    )

