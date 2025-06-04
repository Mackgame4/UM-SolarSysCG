cmake_minimum_required(VERSION 3.10)

project(generator)

include_directories(${PROJECT_NAME} PUBLIC include)

file(GLOB SRC_FILES src/shapes/*.cpp src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} common)

set_target_properties(generator PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
if (UNIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Debug"
    )
endif()