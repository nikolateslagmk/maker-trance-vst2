cmake_minimum_required(VERSION 3.22)

project(MakerTranceVST2 VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Static MSVC runtime: reduces dependency problems on the user's computer.
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

include(FetchContent)
FetchContent_Declare(
    DPF
    GIT_REPOSITORY https://github.com/DISTRHO/DPF.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(DPF)

dpf_add_plugin(MakerTrance
    TARGETS vst2
    FILES_DSP
        Source/MakerTrancePlugin.cpp
    FILES_UI
        Source/MakerTranceUI.cpp
)

target_include_directories(MakerTrance PUBLIC Source)

if(MSVC)
    target_compile_options(MakerTrance PUBLIC /W4 /permissive- /EHsc)
else()
    target_compile_options(MakerTrance PUBLIC -Wall -Wextra -Wpedantic)
endif()
