cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("vs-misc-tomcup")

set(VapourSynthSDK "C:/Users/jilig/Downloads/upscale/VapourSynth/sdk")

add_library(${PROJECT_NAME} SHARED  "vs-misc-tomcup.cpp" )

add_subdirectory(csv-parser)

target_include_directories(${PROJECT_NAME} PRIVATE ${VapourSynthSDK}/include  csv-parser/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${VapourSynthSDK}/lib64)
target_link_libraries(${PROJECT_NAME} VapourSynth.lib csv)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
