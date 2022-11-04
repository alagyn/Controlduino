add_subdirectory(glfw)
add_subdirectory(ViGEmClient)

set(BOOST_ROOT "${PROJECT_SOURCE_DIR}/third_party/boost" CACHE PATH "boost root")
set(Boost_NO_SYSTEM_PATHS "ON" CACHE BOOL "no system")

find_package(Boost 1.80.0
    REQUIRED
)

set(ImGUI_SRCS
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

add_library(ImGUI STATIC ${ImGUI_SRCS})
target_include_directories(ImGUI PUBLIC imgui)
target_link_libraries(ImGUI
    glfw
    opengl32.lib
    gdi32.lib
    shell32.lib
)