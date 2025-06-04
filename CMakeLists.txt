cmake_minimum_required(VERSION 3.10)

project(cg)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Find packages first
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(FreeGLUT CONFIG REQUIRED)

include(FetchContent)

# Dependencies
FetchContent_Declare(fmt GIT_REPOSITORY "https://github.com/fmtlib/fmt")
FetchContent_Declare(glm GIT_REPOSITORY "https://github.com/g-truc/glm")
FetchContent_Declare(imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui"
)

FetchContent_MakeAvailable(fmt glm imgui)

# Custom ImGui with FreeGLUT backend
add_library(CustomImgui STATIC
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glut.cpp  # Note: This should be FreeGLUT
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
)

target_include_directories(CustomImgui PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(CustomImgui PRIVATE 
    $<IF:$<TARGET_EXISTS:FreeGLUT::freeglut>,FreeGLUT::freeglut,FreeGLUT::freeglut_static>
    OpenGL::GL
    GLEW::GLEW
)

# Include the common include directory
include_directories(${CMAKE_SOURCE_DIR}/common/include)

# Add subdirectories
add_subdirectory(common)
add_subdirectory(generator)
add_subdirectory(engine)

# Ensure the generator and engine targets can see the common include directory
target_include_directories(generator PUBLIC ${CMAKE_SOURCE_DIR}/common/include)
# ading also  /lib to the include path
target_include_directories(generator PUBLIC ${CMAKE_SOURCE_DIR}/lib)
# to engine also 
target_include_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/lib)


target_include_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/common/include)
