cmake_minimum_required(VERSION 3.14)

file(GLOB_RECURSE src "*.cpp")
file(GLOB_RECURSE common_src "${CMAKE_SOURCE_DIR}/common/src/*.cpp")
add_executable(orbital-engine ${src} ${common_src})

# target_link_libraries(orbital PRIVATE)

target_include_directories(orbital-engine PRIVATE
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_CURRENT_SOURCE_DIR}/
)

if(WIN32)
    target_compile_definitions(orbital-engine PRIVATE NOGDI NOUSER WIN32_LEAN_AND_MEAN)

    set(PROJECT_DIR ${CMAKE_SOURCE_DIR})
    set(BUILD_DIR ${CMAKE_BINARY_DIR})
    set(CORE_DIR "${BUILD_DIR}/core/${CMAKE_BUILD_TYPE}")

    set(TARGET_DIR "${PROJECT_DIR}")

    add_custom_command(TARGET orbital-engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CORE_DIR}/orbital-engine.exe
        ${TARGET_DIR}/orbital-engine.exe
    )
endif()
