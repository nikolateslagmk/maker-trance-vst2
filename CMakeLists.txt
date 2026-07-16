cmake_minimum_required(VERSION 3.22)

project(MakerTranceVST2 VERSION 1.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# The GitHub workflow checks out DPF into this folder.
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/dpf/CMakeLists.txt")
    message(FATAL_ERROR "DPF not found. Run the included GitHub Actions workflow.")
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

target_include_directories(MakerTrance PUBLIC Source)

if(WIN32)
    if(TARGET "MakerTrance-ui")
        target_link_libraries("MakerTrance-ui" PRIVATE shell32)
    endif()
    if(TARGET "MakerTrance-vst2")
        target_link_libraries("MakerTrance-vst2" PRIVATE shell32)
    endif()
endif()

if(MSVC)
    target_compile_options(MakerTrance PUBLIC /W4 /EHsc)
else()
    target_compile_options(MakerTrance PUBLIC -Wall -Wextra)
endif()

if(TARGET "MakerTrance-vst2")
    set_target_properties(
        "MakerTrance-vst2"
        PROPERTIES
            OUTPUT_NAME "Maker Trance by Alza Produz"
    )
endif()
