cmake_minimum_required(VERSION 3.11) # FetchContent is available in 3.11+
project(zuma-project)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
find_package(flecs)

file(GLOB SOLUTION_SRC src/*.cpp src/*.h)

add_executable(${PROJECT_NAME} ${SOLUTION_SRC})
target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} flecs::flecs_static)
