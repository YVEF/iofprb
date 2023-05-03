cmake_minimum_required(VERSION 3.25)

project(iofprb)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -lblkid")

add_definitions(-DDUMMY_TEST_ENGINE)
set(VERBOSE)
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-Wall -Wextra -Wpedantic -O0 -g)
add_link_options(-lblkid -ludev)


#include_directories(BEFORE /usr/src/linux/include/linux/)

# vcpkg
set(VCPKG_ROOT_PATH "/home/iaroslav/.clion-vcpkg/vcpkg")
set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT_PATH}/scripts/buildsystems/vcpkg.cmake)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# 3rd
find_package(glfw3 3.3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(imgui CONFIG REQUIRED HINTS "${VCPKG_ROOT_PATH}/installed/x64-linux/share/imgui")
find_package(ftxui CONFIG REQUIRED HINTS "${VCPKG_ROOT_PATH}/installed/x64-linux/share/ftxui")
find_package(glad CONFIG REQUIRED HINTS "${VCPKG_ROOT_PATH}/installed/x64-linux/share/glad")
find_package(implot CONFIG REQUIRED HINTS "${VCPKG_ROOT_PATH}/installed/x64-linux/share/implot")


# capture src
file(GLOB_RECURSE utilssrc utils/*.cpp)
file(GLOB_RECURSE uisrc ui/*.cpp)
file(GLOB_RECURSE disksrc disk/*.cpp)
file(GLOB_RECURSE fssrc fs/*.cpp)
file(GLOB_RECURSE prvsrc providers/*.cpp)
file(GLOB_RECURSE jobsrc jobs/*.cpp)

# executable
add_executable(iofprb
        main.cpp
        ${utilssrc}
        ${uisrc}
        ${disksrc}
        ${fssrc}
        ${prvsrc}
        ${jobsrc})

# linking libs
target_link_libraries(iofprb PRIVATE ftxui::dom ftxui::screen ftxui::component)
target_link_libraries(iofprb PRIVATE imgui::imgui)
target_link_libraries(iofprb PRIVATE glad::glad)
target_link_libraries(iofprb PRIVATE glfw)
target_link_libraries(iofprb PRIVATE vulkan)
target_link_libraries(iofprb PRIVATE implot::implot)
