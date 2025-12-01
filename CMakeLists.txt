cmake_minimum_required(VERSION 3.16)

project(sieci_projekt LANGUAGES CXX)

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

set(SIECI_SHARED_SOURCES
    tcp_server.cpp
    tcp_socket.cpp
    tcp_client.cpp
    Error.cpp
    Result.cpp
    logger.cpp
)

add_executable(sieci_server
    main.cpp
    ${SIECI_SHARED_SOURCES}
)

add_executable(server_test
    server_test.cpp
    ${SIECI_SHARED_SOURCES}
)

add_executable(client_test
    client_test.cpp
    ${SIECI_SHARED_SOURCES}
)

foreach(target_name IN ITEMS sieci_server server_test client_test)
    target_include_directories(${target_name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
endforeach()

