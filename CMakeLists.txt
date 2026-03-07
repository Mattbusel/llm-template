cmake_minimum_required(VERSION 3.16)
project(llm_template_examples CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

foreach(ex basic_template loop_template truncated registry)
    add_executable(${ex} examples/${ex}.cpp)
    target_include_directories(${ex} PRIVATE include)
    target_compile_options(${ex} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>
        $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wextra>
    )
endforeach()
