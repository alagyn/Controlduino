cmake_minimum_required(VERSION 3.23)

project(Controlduino)

add_subdirectory(third_party)

set(${PROJECT_NAME}_SRCS
    src/calibration.cpp
    src/controller.cpp
    src/errorUtils.cpp
    src/guiControl.cpp
    src/main.cpp
    src/serializer.cpp
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})
target_include_directories(${PROJECT_NAME}
    PRIVATE
    inc
    arduino/Controlduino
)
target_link_libraries(${PROJECT_NAME}
    glfw
    ImGUI
    ViGEmClient
)

