
add_executable(MotionCube)
target_sources(MotionCube PRIVATE MotionCube.cpp MySDL.cpp)
target_include_directories(MotionCube PRIVATE ${asio_incl})
target_link_libraries(MotionCube PRIVATE glm SDL3::SDL3 GLider::GLider)
if(WIN32)
    target_link_libraries(MotionCube PRIVATE "ws2_32")
endif()

add_executable(TestData)
target_sources(TestData PRIVATE TestData.cpp)
target_include_directories(TestData PRIVATE ${asio_incl})
if(WIN32)
    target_link_libraries(TestData PRIVATE "ws2_32")
endif()