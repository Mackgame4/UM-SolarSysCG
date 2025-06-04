cmake_minimum_required(VERSION 3.10)

project(engine)

file(GLOB SRC_FILES src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC include)

# Link common library first
target_link_libraries(${PROJECT_NAME} PRIVATE common)

# Link other libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    $<IF:$<TARGET_EXISTS:FreeGLUT::freeglut>,FreeGLUT::freeglut,FreeGLUT::freeglut_static>
    OpenGL::GL
    GLEW::GLEW
    CustomImgui
    fmt::fmt
    glm::glm
)

if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE GLU)
endif()

if (UNIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${PROJECT_NAME}/Debug"
    )
endif()

set_target_properties(engine PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)