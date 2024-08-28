cmake_minimum_required(VERSION 3.11)
project(game LANGUAGES CXX)

include(cmake/CPM.cmake)

CPMAddPackage(
        NAME flecs
        VERSION 4.0.0
        GITHUB_REPOSITORY SanderMertens/flecs
)

CPMAddPackage(
        NAME raylib
        GIT_TAG 5.0
        GITHUB_REPOSITORY raysan5/raylib
)

file(GLOB SOLUTION_SRC src/*.cpp)
file(GLOB COMPONENTS src/ecs/*.cpp)
file(GLOB GLOBAL src/global/*.cpp)
file(GLOB LIB src/lib/*.cpp)



add_executable(${PROJECT_NAME}
        ${SOLUTION_SRC}
        ${COMPONENTS}
        ${GLOBAL}
        ${LIB})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/ecs)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/global)

target_link_libraries(${PROJECT_NAME} flecs::flecs_static)
target_link_libraries(${PROJECT_NAME} raylib)
