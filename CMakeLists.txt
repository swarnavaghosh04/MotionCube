
cmake_minimum_required(VERSION 3.19)
project(MotionCube VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE
		STRING "Choose the type of build." FORCE
	)
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
else()
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

if(NOT APPLE)
	set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

find_package(GLider 0.1.0 REQUIRED)
find_path(asio_incl NAMES asio.hpp)

include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/${CMAKE_INSTALL_BINDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/${CMAKE_INSTALL_LIBDIR})

set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/${CMAKE_INSTALL_BINDIR})
set(ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/${CMAKE_INSTALL_LIBDIR})
set(LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/${CMAKE_INSTALL_LIBDIR})

string(
    APPEND _opts
    "$<IF:$<CXX_COMPILER_ID:MSVC>,"
        "/W4;$<$<CONFIG:RELEASE>:/O2>,"
        "-Wall;-Wextra;-Werror;"
            "$<$<CONFIG:RELEASE>:-O3>"
    ">"
)
add_compile_options("${_opts}")

add_executable(MotionCube)

target_sources(MotionCube PRIVATE cube.cpp)
target_include_directories(MotionCube PRIVATE ${asio_incl})
target_link_libraries(MotionCube PRIVATE GLider::GLider "ws2_32")