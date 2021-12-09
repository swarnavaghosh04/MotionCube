
cmake_minimum_required(VERSION 3.19)
project(MotionCube VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE
		STRING "Choose the type of build." FORCE
	)
endif()

if(NOT APPLE)
	set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_INSTALL_BINDIR})
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_subdirectory(extern)

find_package(GLider CONFIG REQUIRED)
find_package(SDL2 CONFIG)
find_package(glm CONFIG REQUIRED)
find_path(asio_incl NAMES asio.hpp)

if(NOT TARGET SDL2::SDL2)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SDL2ConfigBackup.cmake)
endif()

string(
    APPEND _opts
    "$<IF:$<CXX_COMPILER_ID:MSVC>,"
        "/W4;$<$<CONFIG:RELEASE>:/O2>,"
        "-Wall;-Wextra;"
            "$<$<CONFIG:RELEASE>:-O3>"
    ">"
)
add_compile_options("${_opts}")

add_subdirectory(src)

install(
	TARGETS MotionCube TestData
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT MotionCube_Runtime
)
install(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/share/MotionCube
	DESTINATION ${CMAKE_INSTALL_DATADIR}
)