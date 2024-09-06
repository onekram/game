cmake_minimum_required(VERSION 3.24)

set(CPM_VERSION 0.38.1)
set(CPM_SOURCE_CACHE ${CMAKE_SOURCE_DIR}/ext)
set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_VERSION}/CPM.cmake
            ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

if(NOT DEFINED PLATFORM)
    set(PLATFORM Desktop)
endif()

dd add_definitions(-DPATHDEF="${CMAKE_SOURCE_DIR}")

if (${PLATFORM} STREQUAL Web)
    add_definitions(-DPLATFORM_WEB)
    set(EMSDK_VERSION 3.1.45)
    CPMAddPackage("gh:emscripten-core/emsdk#${EMSDK_VERSION}")

    if(NOT (EXISTS ${emsdk_SOURCE_DIR}/upstream))
        message("Activating emsdk")

        if(WIN32)
            set(EMSDK_CMD "${emsdk_SOURCE_DIR}/emsdk.bat")
        else()
            set(EMSDK_CMD "${emsdk_SOURCE_DIR}/emsdk")
        endif()

        execute_process(COMMAND ${EMSDK_CMD} install ${EMSDK_VERSION})
        execute_process(COMMAND ${EMSDK_CMD} activate ${EMSDK_VERSION})
    endif()

    set(CMAKE_TOOLCHAIN_FILE ${emsdk_SOURCE_DIR}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake)
endif()


project(game VERSION 0.0.1 LANGUAGES C CXX)

file(GLOB_RECURSE SOLUTION_SRC src/*.cpp)

add_executable(${PROJECT_NAME} ${SOLUTION_SRC})

CPMAddPackage(NAME raylib
        GITHUB_REPOSITORY raysan5/raylib
        GIT_TAG 5.0
        OPTIONS "PLATFORM ${PLATFORM}" "CMAKE_BUILD_TYPE MinSizeRel")

target_include_directories(${PROJECT_NAME} PRIVATE ${raylib_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)

if(${PLATFORM} STREQUAL Web)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            LINK_FLAGS "--preload-file ${CMAKE_SOURCE_DIR}/icons --preload-file ${CMAKE_SOURCE_DIR}/sounds -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s TOTAL_MEMORY=67108864 -s ALLOW_MEMORY_GROWTH=1 -s STACK_SIZE=1mb "
            OUTPUT_NAME "index"
            SUFFIX ".html")
endif()


CPMAddPackage(
        NAME flecs
        VERSION 4.0.0
        GITHUB_REPOSITORY SanderMertens/flecs
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/ecs)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/global)

target_link_libraries(${PROJECT_NAME} PRIVATE flecs::flecs_static)
