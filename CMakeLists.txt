cmake_minimum_required(VERSION 3.22)

project(MakerTranceVST2 VERSION 1.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

# Compatibilidade OpenGL no Windows.
# Baixa o cabeçalho oficial da Khronos que contém os tipos
# PFNGLACTIVETEXTUREPROC, PFNGLATTACHSHADERPROC etc.
set(OPENGL_COMPAT_DIR "${CMAKE_BINARY_DIR}/opengl-compat")
set(OPENGL_COMPAT_GL_DIR "${OPENGL_COMPAT_DIR}/GL")
set(OPENGL_GLEXT_HEADER "${OPENGL_COMPAT_GL_DIR}/glext.h")

file(MAKE_DIRECTORY "${OPENGL_COMPAT_GL_DIR}")

file(
    DOWNLOAD
    "https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/main/api/GL/glext.h"
    "${OPENGL_GLEXT_HEADER}"
    TLS_VERIFY ON
    SHOW_PROGRESS
    STATUS GLEXT_DOWNLOAD_STATUS
)

list(GET GLEXT_DOWNLOAD_STATUS 0 GLEXT_DOWNLOAD_CODE)
list(GET GLEXT_DOWNLOAD_STATUS 1 GLEXT_DOWNLOAD_MESSAGE)

if(NOT GLEXT_DOWNLOAD_CODE EQUAL 0)
    message(
        FATAL_ERROR
        "Falha ao baixar GL/glext.h: ${GLEXT_DOWNLOAD_MESSAGE}"
    )
endif()

include_directories(BEFORE "${OPENGL_COMPAT_DIR}")

include(FetchContent)

FetchContent_Declare(
    DPF
    GIT_REPOSITORY https://github.com/DISTRHO/DPF.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(DPF)

dpf_add_plugin(
    MakerTrance

    TARGETS
        vst2

    UI_TYPE
        opengl

    FILES_DSP
        Source/MakerTrancePlugin.cpp

    FILES_UI
        Source/MakerTranceUI.cpp
)

target_include_directories(
    MakerTrance
    PUBLIC
        Source
)

# Garante que o próprio motor visual do DPF encontre
# primeiro o cabeçalho OpenGL baixado acima.
if(TARGET dgl-opengl)
    target_include_directories(
        dgl-opengl
        BEFORE PRIVATE
            "${OPENGL_COMPAT_DIR}"
    )
endif()

if(MSVC)
    target_compile_options(
        MakerTrance
        PUBLIC
            /W4
            /EHsc
    )
else()
    target_compile_options(
        MakerTrance
        PUBLIC
            -Wall
            -Wextra
            -Wpedantic
    )
endif()
