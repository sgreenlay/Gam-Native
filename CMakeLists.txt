cmake_minimum_required(VERSION 3.5)

if(CMAKE_SYSTEM_NAME STREQUAL "Binaryen" AND NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/toolchains/web.cmake" CACHE STRING "")
    set(BUILD_SHARED_LIBS ON CACHE BOOL "")
endif()

project(gam CXX)

## ENGINE
file(GLOB_RECURSE ENGINE_SOURCES engine/src/*.cpp)
add_library(engine ${ENGINE_SOURCES})
target_include_directories(engine PUBLIC engine/inc)

## GAM
if(NOT CMAKE_SYSTEM_NAME STREQUAL "Binaryen")
    file(GLOB_RECURSE WINDOWS_SOURCES shared/windows/*.cpp)
    file(GLOB_RECURSE WIN32_SOURCES targets/win32/*.cpp)

    set(GAM_SOURCES)
    set(GAM_IS_WIN32)

    if(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
        list(APPEND GAM_SOURCES ${WINDOWS_SOURCES})
    elseif(WIN32)
        list(APPEND GAM_SOURCES ${WIN32_SOURCES} ${WINDOWS_SOURCES})
        set(GAM_IS_WIN32 WIN32)
    endif()

    add_executable(gam ${GAM_IS_WIN32} ${GAM_SOURCES})
    target_link_libraries(gam PRIVATE engine)

    if(CMAKE_SYSTEM_NAME STREQUAL "WindowsStore")
        target_include_directories(gam PRIVATE shared/windows)
    elseif(WIN32)
        target_include_directories(gam PRIVATE shared/windows targets/win32)
        target_link_libraries(gam PRIVATE imm32)
    endif()
else()
    configure_file(targets/web/index.html index.html COPYONLY)
endif()
