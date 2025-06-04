cmake_minimum_required(VERSION 3.10)

project(common)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(GLOB SRC_FILES src/*.cpp)
add_library(${PROJECT_NAME} ${SRC_FILES})
set_target_properties(common PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
