cmake_minimum_required(VERSION 3.22)

project(MakerTranceVST2 VERSION 1.0.3 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(MSVC)
    set(
        CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

# O workflow baixa o DPF para esta pasta.
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/dpf/CMakeLists.txt")
    message(
        FATAL_ERROR
        "A pasta dpf nao foi encontrada. Execute o workflow do GitHub Actions."
    )
endif()

add_subdirectory(dpf)

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
    )
endif()

if(TARGET "MakerTrance-vst2")
    set_target_properties(
        "MakerTrance-vst2"
        PROPERTIES
            OUTPUT_NAME "Maker Trance by Alza Produz"
    )
endif()
