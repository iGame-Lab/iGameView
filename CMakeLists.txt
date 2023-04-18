cmake_minimum_required(VERSION 3.19.2)
project(iGameView)

set(CMAKE_CXX_STANDARD 14)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O2 -pthread")
#添加宏定义
add_definitions(/D_USE_MATH_DEFINES)

#编译器
# linux平台的GCC编译器
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    message(STATUS "is gcc")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -O2 -pthread")
    #设置QT的目录
    set(QT_CMAKE_PATH "C:/Users/ASUS/Desktop/Dev/Dependence/QT/5.12.3/msvc2017_64/lib/cmake")
    set(Qt5Core_DIR ${QT_CMAKE_PATH}/Qt5Core)
    set(Qt5Gui_DIR ${QT_CMAKE_PATH}/Qt5Gui)
    set(Qt5OpenGL_DIR ${QT_CMAKE_PATH}/Qt5OpenGL)
    set(Qt5Widgets_DIR ${QT_CMAKE_PATH}/Qt5Widgets)

    #查找GLUT
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIR})

# Windows平台的MSVC编译器
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    message(STATUS "编译器是msvc")
    #设置QT的目录
    set(QT_CMAKE_PATH "C:/Users/ASUS/Desktop/Dev/Dependence/QT/5.12.3/msvc2017_64/lib/cmake")
    set(Qt5Core_DIR ${QT_CMAKE_PATH}/Qt5Core)
    set(Qt5Gui_DIR ${QT_CMAKE_PATH}/Qt5Gui)
    set(Qt5OpenGL_DIR ${QT_CMAKE_PATH}/Qt5OpenGL)
    set(Qt5Widgets_DIR ${QT_CMAKE_PATH}/Qt5Widgets)

    #查找GLUT
    SET(GLUT_INCLUDE_DIR extern/GLUT/include)
    SET(GLUT_LIBRARY extern/GLUT/lib/glut32.lib)
    include_directories(${GLUT_INCLUDE_DIR})
    include_directories(${GLUT_LIB_DIR})

# MacOS平台的Clang编译器
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    message(STATUS "编译器是AppleClang")
    #设置QT的目录
    set(QT_CMAKE_PATH "C:/Users/ASUS/Desktop/Dev/Dependence/QT/5.12.3/msvc2017_64/lib/cmake")
    set(Qt5Core_DIR ${QT_CMAKE_PATH}/Qt5Core)
    set(Qt5Gui_DIR ${QT_CMAKE_PATH}/Qt5Gui)
    set(Qt5OpenGL_DIR ${QT_CMAKE_PATH}/Qt5OpenGL)
    set(Qt5Widgets_DIR ${QT_CMAKE_PATH}/Qt5Widgets)

    #查找GLUT
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIR})

endif()

# 寻找系统中下载的三方库
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

#path
SET(EIGEN_PATH "${PROJECT_SOURCE_DIR}/extern/")
#SET(OPENMESH_PATH "/opt/homebrew/Cellar/open-mesh/8.1/include")
#SET(OPENMESH_LIB_PATH "/opt/homebrew/Cellar/open-mesh/8.1/lib")

#QT
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5OpenGL REQUIRED)
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)	# https://zhuanlan.zhihu.com/p/73469206，调用windepolyqt进行自动部署
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(DEPLOYQT_EXECUTABLE NAMES windeployqt macdeployqt HINTS "${_qt_bin_dir}")

set(QRC_SOURCE_FILES Turbine_Blade.qrc)
qt5_add_resources(QRC_FILES ${QRC_SOURCE_FILES})
set(UI_SOURCE_FILES Turbine_Blade.ui)
qt5_wrap_ui(UI_FILES ${UI_SOURCE_FILES})

include_directories(${EIGEN_PATH})
#include_directories(${OPENMESH_PATH})
#link_directories(${OPENMESH_LIB_PATH})

include_directories(${PROJECT_SOURCE_DIR}/extern/)


file (GLOB GUI_FILE
        main.cpp
        iGameMeshView.*
        ModelDraw.*
        PrintInfo.*
        ViewOpenGL.*
        )

file (GLOB Mesh_FILE
        TinyVector.h
        NurbsSDK/*
        NurbsSDK/tinyXML/*
        )


file (GLOB Tet_FILE
        Mesh/Tetrahedron/*
        )

file (GLOB Ex_FILE
        extern/include/*
        )


# 必要3 项目中添加执行文件
add_executable(${PROJECT_NAME} ${SRCFILES}
        ${GUI_FILE}
        ${Mesh_FILE}
        ${Tet_FILE}
        ${QRC_FILES}
        ${UI_FILES}
        ${Ex_FILE}
        )

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${DEPLOYQT_EXECUTABLE} "$<TARGET_FILE:${PROJECT_NAME}>"
            --$<LOWER_CASE:$<CONFIG>> --qmldir ${CMAKE_SOURCE_DIR}
            )
endif()

source_group(GUI FILES ${GUI_FILE})
source_group(Mesh FILES ${Mesh_FILE})

target_include_directories(${PROJECT_NAME} PUBLIC
        extern/LibMeshFix/include/TMesh
        extern/LibMeshFix/include/Kernel
        )

target_compile_definitions(${PROJECT_NAME} PUBLIC -DIS64BITPLATFORM)

target_link_libraries(${PROJECT_NAME}
        Qt5::Widgets
        Qt5::Core
        Qt5::Gui
        Qt5::OpenGL
#        optimized OpenMeshCore
#        optimized OpenMeshTools
#        debug OpenMeshCored
#        debug OpenMeshToolsd
        )
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Release"))
        message("Release Mode")
        file (GLOB QUAD_FILE
                extern/include/*
                )
        include_directories(${QUAD_FILE})
        file (GLOB QUAD_LIB_FILE
                extern/lib/*
                )
    endif()
    target_link_libraries(${PROJECT_NAME}
            opengl32
            glu32
            ${QUAD_LIB_FILE}
            )
else()
    include_directories(${PROJECT_SOURCE_DIR})
    target_link_libraries(${PROJECT_NAME}
            ${GLUT_LIBRARY}
            ${OPENGL_LIBRARY}
            #-lgmp
            )
endif()

