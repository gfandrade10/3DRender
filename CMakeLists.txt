cmake_minimum_required(VERSION 3.16)
project(OpenGLPhysics)
set(INC ${CMAKE_SOURCE_DIR}/includes)
set(IMGUI "F:/Codes/conan_data/p/imgui6d92dd284f976/s/src")

add_subdirectory(src)

find_package(OpenGL REQUIRED)
find_package(imgui REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Threads REQUIRED)
find_package(glad REQUIRED)
find_package(stb REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${INC} ${IMGUI})

target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    imgui::imgui
    glfw
    glm::glm
    Threads::Threads
    glad::glad
    stb::stb
)

if(DBGMODE)
    target_compile_options(${PROJECT_NAME} PRIVATE "-g")
endif()
