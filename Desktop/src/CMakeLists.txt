
add_executable(MotionCube)
target_sources(MotionCube PRIVATE MotionCubeDesktop.cpp MySDL.cpp)
target_include_directories(MotionCube PRIVATE ${asio_incl})
target_link_libraries(MotionCube PRIVATE glm SDL2::SDL2 GLider::GLider "ws2_32")