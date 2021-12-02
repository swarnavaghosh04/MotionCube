
add_executable(MotionCubeDesktop)
target_sources(MotionCubeDesktop PRIVATE MotionCubeDesktop.cpp MySDL.cpp)
target_include_directories(MotionCubeDesktop PRIVATE ${asio_incl})
target_link_libraries(MotionCubeDesktop PRIVATE glm SDL2::SDL2 GLider::GLider "ws2_32")

add_executable(TestData)
target_sources(TestData PRIVATE TestData.cpp)
target_include_directories(TestData PRIVATE ${asio_incl})
target_link_libraries(TestData PRIVATE "ws2_32")