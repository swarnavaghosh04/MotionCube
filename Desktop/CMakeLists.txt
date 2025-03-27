
cmake_minimum_required(VERSION 3.19)
project(MotionCube VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_INSTALL_BINDIR})
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

find_package(GLider CONFIG REQUIRED)
find_package(SDL3 CONFIG)
find_package(glm CONFIG REQUIRED)
find_path(asio_incl NAMES asio.hpp)

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