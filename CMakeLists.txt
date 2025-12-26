cmake_minimum_required(VERSION 3.16)

project(wordle-server LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(default_build_type "Debug")
    message(STATUS "No build type selected, defaulting to ${default_build_type}")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -fno-omit-frame-pointer")

add_subdirectory(external/nlohmann_json)

find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE SIECI_SHARED_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/server/server/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/server/web-socket/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/server/http/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/server/utils/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/server/cron/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/logic/*.cpp"
)

add_executable(wordle-server
    main.cpp
    ${SIECI_SHARED_SOURCES}
)


set(PROJECT_TARGETS wordle-server)

foreach(target_name IN LISTS PROJECT_TARGETS)
    if(TARGET ${target_name})
        target_include_directories(${target_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
        target_link_libraries(${target_name} PRIVATE nlohmann_json::nlohmann_json OpenSSL::SSL OpenSSL::Crypto)
    endif()
endforeach()

