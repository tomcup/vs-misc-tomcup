cmake_minimum_required (VERSION 3.8)

project ("vs-misc-tomcup")

set(VapourSynthSDK "C:/Users/jilig/Downloads/upscale/VapourSynth/sdk")

add_library(${PROJECT_NAME} SHARED  "MiscTomcup.cpp")

add_subdirectory(csv-parser)

target_include_directories(${PROJECT_NAME} PRIVATE ${VapourSynthSDK}/include ${PROJECT_SOURCE_DIR}/csv-parser/include)
target_link_libraries(${PROJECT_NAME} csv)

option(BUILD_TEST "Whether or not to build the tests" ON)
if (${BUILD_TEST})
    message(STATUS "Building tests")
    add_subdirectory(tests)
endif ()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
