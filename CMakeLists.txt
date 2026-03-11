cmake_minimum_required(VERSION 3.10..3.20)
project(
        cement_plant_simulator
        VERSION 1.0.0
        DESCRIPTION ""
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

option(EXTRA_WARNING "build with warnings as errors" OFF)
option(BUILD_APP "build app" ON)
option(BUILD_TESTS "build tests" OFF)
option(BUILD_BENCHMARK "build benchmarks" OFF)
option(BUILD_SHARED "The handler as shared library" ON)

if(EXTRA_WARNING)
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
    elseif(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W3 -WX")
    endif()
endif()

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

add_subdirectory(src)

if(BUILD_APP)
    add_subdirectory(app)
endif ()

#if(BUILD_TESTS)
#    find_package(GTest CONFIG REQUIRED)
#    if(GTest_FOUND)
#        enable_testing()
#        add_subdirectory(tests)
#    endif ()
#endif ()

if(BUILD_BENCHMARK)
    find_package(benchmark CONFIG REQUIRED)
    if(benchmark_FOUND)
        add_subdirectory(benchmark)
    endif ()
endif ()
