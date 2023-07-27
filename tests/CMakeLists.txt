add_executable(${PROJECT_NAME}Test "test.cpp")

target_include_directories(${PROJECT_NAME}Test PRIVATE ${VapourSynthSDK}/include ${PROJECT_SOURCE_DIR}/csv-parser/include)
target_link_libraries(${PROJECT_NAME}Test csv)

set_property(TARGET ${PROJECT_NAME}Test PROPERTY CXX_STANDARD 20)