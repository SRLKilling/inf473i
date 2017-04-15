CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(OpenCV REQUIRED)

PROJECT(proj)

ADD_EXECUTABLE(proj main.cpp scene.cpp scene.hpp lkt_scene.cpp lkt_scene.hpp roi_scene.cpp roi_scene.hpp ccontour_scene.cpp ccontour_scene.hpp)
TARGET_LINK_LIBRARIES(proj ${OpenCV_LIBS})
