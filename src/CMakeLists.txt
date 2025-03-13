file(GLOB LOCAL_SOURCES *.cpp)

set(SOURCES
    ${LOCAL_SOURCES}
    ${IMGUI}/imgui.cpp
    ${IMGUI}/imgui_draw.cpp
    ${IMGUI}/imgui_tables.cpp
    ${IMGUI}/imgui_widgets.cpp
    # Platform implementations
    ${IMGUI}/backends/imgui_impl_glfw.cpp
    ${IMGUI}/backends/imgui_impl_opengl3.cpp 
)

add_executable(${PROJECT_NAME} ${SOURCES})
