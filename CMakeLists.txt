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
file(GLOB COMPONENTS components/*.cpp)
file(GLOB GLOBAL global/*.cpp)
file(GLOB LIB lib/*.cpp)



add_executable(${PROJECT_NAME}
        ${SOLUTION_SRC}
        ${COMPONENTS}
        ${GLOBAL}
        ${LIB})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/components)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/global)

target_link_libraries(${PROJECT_NAME} flecs::flecs_static)
target_link_libraries(${PROJECT_NAME} raylib)
