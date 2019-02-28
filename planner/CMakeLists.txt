# CMakeList.txt : CMake project for planner, include source and define
# project specific logic here.
# With help from https://github.com/preshing/CMakeDemo/blob/master/CMakeLists.txt

cmake_minimum_required (VERSION 3.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
find_package(SDL2 REQUIRED COMPONENTS main)

include_directories(${SDL2_INCLUDE_DIRS} ${SDL2main_INCLUDE_DIRS} ${CMAKE_BINARY_DIR})

# Add source to this project's executable.
add_executable (planner "planner.cpp" "planner.h")

target_link_libraries(planner ${SDL2_LIBS})


if(WIN32)
    foreach(DLL ${SDL2_DLLS})
        add_custom_command(TARGET planner POST_BUILD COMMAND
            ${CMAKE_COMMAND} -E copy_if_different ${DLL} $<TARGET_FILE_DIR:planner>)
    endforeach()
endif()
# TODO: Add tests and install targets if needed.
