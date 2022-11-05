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
    src/readManager.cpp
)

find_package(Boost 1.81.0
    REQUIRED
    COMPONENTS system regex
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRCS})
target_include_directories(${PROJECT_NAME}
    PRIVATE
    inc
    arduino/Controlduino
    PUBLIC
    ${Boost_INCLUDE_DIRS}
)

message("Libs: ${Boost_LIBRARIES}")
message("Include: ${Boost_INCLUDE_DIRS}")

target_link_libraries(${PROJECT_NAME}
    glfw
    ImGUI
    ViGEmClient
    ${Boost_LIBRARIES}
)

