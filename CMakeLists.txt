cmake_minimum_required (VERSION 3.8)

project ("vs-misc-tomcup")

set(VapourSynthSDK "C:/Users/jilig/Downloads/upscale/VapourSynth/sdk")

add_library(${PROJECT_NAME} SHARED  "MiscTomcup.cpp")

add_subdirectory(csv-parser)

target_include_directories(${PROJECT_NAME} PRIVATE ${VapourSynthSDK}/include csv-parser/include)
target_link_libraries(${PROJECT_NAME} csv)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
