file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE FORMS ${CMAKE_CURRENT_SOURCE_DIR}/*.ui)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

qt_standard_project_setup()

if (BUILD_SHARED)
    add_library(gui_${PROJECT_NAME} SHARED ${HEADERS} ${SOURCES} ${FORMS})
else ()
    add_library(gui_${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} ${FORMS})
endif()

target_link_libraries(gui_${PROJECT_NAME} PUBLIC Qt6::Core Qt6::Widgets)
target_include_directories(gui_${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Widgets_INCLUDE_DIRS}
    )

