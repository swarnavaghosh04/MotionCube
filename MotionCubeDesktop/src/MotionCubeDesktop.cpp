/* ============================================
The MIT License (MIT)

Copyright (c) 2021 Swarnava Ghosh

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================*/

#define GLM_ENABLE_EXPERIMENTAL

#include <GLider/GLider.hpp>
#include <glm/gtx/transform.hpp>
#include <asio.hpp>
#include <cstdio>
#include <thread>
#include <atomic>
#include <array>
#include <chrono>
#include <stdexcept>
#include "MySDL.hpp"

std::atomic<bool> keepRunning{true};

static std::exception_ptr teptr = nullptr;

struct Cube{

    static constexpr const char* const vertexShader = R"CODE(
        #version 330 core

        layout(location=0) in vec4 position;

        uniform mat4 u_mat;
        uniform vec4 orientation;
        uniform vec4 def_orientation;

        vec4 quat_mult(vec4 a, vec4 b)
        {
            return vec4(
                ((a.w*b.x) + (a.x*b.w) + (a.y*b.z) - (a.z*b.y)), // x
                ((a.w*b.y) - (a.x*b.z) + (a.y*b.w) + (a.z*b.x)), // y
                ((a.w*b.z) + (a.x*b.y) - (a.y*b.x) + (a.z*b.w)), // z
                ((a.w*b.w) - (a.x*b.x) - (a.y*b.y) - (a.z*b.z))  // w
            );
        }

        vec4 quat_conj(vec4 q)
        {
            return vec4(-q.x, -q.y, -q.z, q.w);
        }

        vec4 quat_inv(vec4 q)
        {
            float mag_sqr = q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
            return quat_conj(q)/mag_sqr;
        }

        vec4 quat_rot(float angle, vec3 axis){
            float half_angle = angle/2;
            float sin_half_angle = sin(half_angle);
            return vec4(
                axis.x * sin_half_angle,
                axis.y * sin_half_angle,
                axis.z * sin_half_angle,
                cos(half_angle)
            );
        }

        vec4 quat_rot(vec4 quaternion, vec4 rotation){
            return quat_mult(
                rotation,
                quat_mult(
                    quaternion,
                    quat_conj(rotation)
                )
            );
        }

        void main(){

            // Calibration
            vec4 correct_rot = quat_mult(quat_inv(def_orientation), orientation);

            // Actual orientation
            gl_Position = u_mat * quat_rot(position, correct_rot);

        }
    )CODE";

    static constexpr const char* const fragmentShader = R"CODE(
        #version 330 core

        uniform vec4 u_color;

        void main(){
            gl_FragColor = u_color;
        }
    )CODE";

    std::array<float,3*8>            vertexBufData;
    std::array<unsigned char, 6*6>   indexBufData;

    gli::VertexArray                        vertexArray;
    gli::Buffer<gli::VertexBuffer>          vertexBuffer;
    gli::Buffer<gli::IndexBuffer>           indexBuffer;
    gli::ShaderProgram                      shaders;
    glm::vec4                               orientation;
    std::mutex                              orientation_mutex;

    asio::io_service io;
    asio::serial_port serial;

    Cube():
        io{},
        serial{io}
    {
        SDL_Log("Cube Init\n");
        generateVertices(2.f,0.5f,1.f, glm::vec3(0,0,0), vertexBufData, indexBufData);
        vertexBuffer.feedData(vertexBufData, gli::UseStaticDraw);
        indexBuffer.feedData(indexBufData, gli::UseStaticDraw);
        gli::Layout layout(1);
        layout.push<float>(gli::D3, false);
        vertexArray.readBufferData(vertexBuffer, layout);
        shaders.compileString(gli::VertexShader, vertexShader);
        shaders.compileString(gli::FragmentShader, fragmentShader);
        shaders.link();
        shaders.validate();
        shaders.bind();
        vertexArray.bind();
        indexBuffer.bind();
        shaders.setUniform("def_orientation", glm::vec4(0.f, 0.f, 0.f, 1.f));
    }

    ~Cube(){
        if(serial.is_open()) serial.close();
    }

    template <std::size_t N1, std::size_t N2>
    static void generateVertices(
        float XLength,
        float YLength,
        float ZLength,
        const glm::vec3& center,
        std::array<float,N1>& vertexOutputBuffer,
        std::array<unsigned char, N2>& indexOutputBuffer
    ){

        glm::vec2 a(center.x-XLength/2.f, center.x+XLength/2.f);
        glm::vec2 b(center.y-YLength/2.f, center.y+YLength/2.f);
        glm::vec2 c(center.z-ZLength/2.f, center.z+ZLength/2.f);

        {
            const float vertices[3*8] = {
                a.x, b.x, c.x,
                a.x, b.x, c.y,
                a.x, b.y, c.y,
                a.x, b.y, c.x,
                a.y, b.x, c.x,
                a.y, b.x, c.y,
                a.y, b.y, c.y,
                a.y, b.y, c.x,
            };
            memcpy(vertexOutputBuffer.data(), vertices, sizeof(vertices));
        }{
            const unsigned char indices[6*6] = {
                0, 1, 3, 1, 3, 2,
                6, 5, 7, 5, 7, 4,
                1, 0, 5, 0, 5, 4,
                2, 3, 6, 3, 6, 7,
                0, 3, 4, 3, 4, 7,
                2, 1, 6, 1, 6, 5
            };
            memcpy(indexOutputBuffer.data(), indices, sizeof(indices));
        }
    }

    void setupSerial(const char* port){

        serial.open(port);
        if(!serial.is_open())
            throw std::runtime_error("Serial cannot be opened");

        serial.set_option(
            asio::serial_port_base::baud_rate(115200)
        );
        serial.set_option(
            asio::serial_port_base::flow_control(
                asio::serial_port_base::flow_control::none
            )
        );
        serial.set_option(
            asio::serial_port_base::character_size(8)
        );
        serial.set_option(
            asio::serial_port_base::parity(
                asio::serial_port_base::parity::even
            )
        );
        serial.set_option(
            asio::serial_port_base::stop_bits(
                asio::serial_port_base::stop_bits::one
            )
        );

        SDL_Delay(100);
        char c;
        SDL_Log("Trying to contact...");
        asio::read(serial, asio::buffer(&c, 1));
        while(c != 0)
            asio::read(serial, asio::buffer(&c, 1));
        asio::read(serial, asio::buffer(&c, 1));
        if(c != 0) throw std::runtime_error("Cannot Initialize MPU DMP");
        SDL_Log("MPU DMP Initialized");
    }

    void draw()
    {
        int drawCounter = 0;

        vertexArray.bind();

        auto draw_square_with_color = [&](const glm::vec4& v){

            shaders.setUniform("u_color", v);
            indexBuffer.draw<unsigned char>(
                gli::DrawTriangles,
                6,
                drawCounter);
            drawCounter += 6;
            indexBuffer.draw<unsigned char>(
                gli::DrawTriangles,
                6,
                drawCounter);
            drawCounter += 6;
        };

        draw_square_with_color(glm::vec4(1.0, 0.0, 0.0, 1.0));
        draw_square_with_color(glm::vec4(0.0, 1.0, 0.0, 1.0));
        draw_square_with_color(glm::vec4(0.0, 0.0, 1.0, 1.0));
    }

    void updateOrientation(){

        // Arduino data format ->       w, x, y, z
        // Orientation data format ->   x, y, z, w

        // Orientation X -> Gyro Y
        // Orientation Y -> Gyro Z
        // Orientation Z -> Gyro X

        struct{
            float w,x,y,z; // arduino data format
        } gyro;

        // Get the data
        static constexpr const char c = 1;
        asio::write(serial, asio::buffer(&c, 1));
        asio::read(serial, asio::buffer(&gyro, sizeof(gyro)));

        // Re-interpret the data
        const std::lock_guard lck(orientation_mutex);
        orientation.x = gyro.y;
        orientation.y = gyro.z;
        orientation.z = gyro.x;
        orientation.w = gyro.w;
    }

    void readDataFromPort(){
        try{
            while(keepRunning)
                updateOrientation();
        }catch(...){
            teptr = std::current_exception();
        }
    }

};

static int get_appropriate_window_dimension(){

    SDL_DisplayMode dm;

    if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
        throw std::runtime_error(SDL_GetError());

    return int((float)std::min(dm.h, dm.w) * 3.f/4.f);
    
}

int main(int argc, const char* argv[]){

    if(argc < 2){
        std::printf("Please Enter Serial Port\n");
        return 1;
    }

    int return_value = 0;

    try{
        
        SDL sdl(3,3);

        int dim = get_appropriate_window_dimension();
        SDL::OpenGLWindow window{"Cube", dim, dim};

        gli::initialize(SDL_GL_GetProcAddress);

        gli::enable(gli::Capability_NI::DepthTest);
        gli::depthRange(0.01, 1000.0);
        
        SDL_Log("GLVerion: %d.%d\n", GLVersion.major, GLVersion.minor);

        Cube cube;
        cube.setupSerial(argv[1]);

        // Setup MVP =============================

        glm::mat4 mvp = 
            glm::perspective(
                70.f*3.14159f/180.f,    // fovy
                1.f,                    // aspect_ratio (1 for square)
                0.001f, 1000.f
            )*glm::lookAt(
                glm::vec3(0.f, 0.f, 5.f),   // Looking from
                glm::vec3(0.f, 0.f, 0.4),   // Looking at
                glm::vec3(0.f, 1.f, 0.f)    // Up
            );
        
        cube.shaders.setUniform("u_mat", mvp, false);

        gli::depthRange(0.01, 1000.0);

        std::thread readingThread(&Cube::readDataFromPort, std::ref(cube));
        
        gli::FrameRate fps;
        auto fps_print = std::chrono::steady_clock::now();

        SDL_Event event;

        /* Loop until the user closes the window */
        for(
            std::unique_lock
                orientation_lock(cube.orientation_mutex, std::defer_lock);
            keepRunning;
        ){

            orientation_lock.lock();
            cube.shaders.setUniform("orientation", cube.orientation);
            orientation_lock.unlock();

            // cube.shaders.setUniform("orientation", glm::vec4(0,0,0,1));

            gli::clear(gli::ColorBufferBit | gli::DepthBufferBit);

            cube.draw();

            window.swap();

            while(SDL_PollEvent(&event)){
                
                #define shiftModifiersActived event.key.keysym.mod & KMOD_SHIFT

                switch(event.type){
                case SDL_QUIT:
                    keepRunning = false;
                    break;
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_SPACE)
                        cube.shaders.setUniform("def_orientation", cube.orientation);
                    break;
                }

                #undef shiftModifiersActived

            }

            fps.compute();

            if(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - fps_print
                ).count() >= 500
            ){
                printf(
                    "fps: %5.0f\r",
                    fps()
                );
                fps_print = std::chrono::steady_clock::now();
            }
            
        } // for(;keepRunning;)

        readingThread.join();

        if(teptr) std::rethrow_exception(teptr);

    }catch(const std::exception& ex){
        std::printf("%s occured!\n", typeid(ex).name());
        std::printf(ex.what());
        return_value = 2;
    }catch(...){
        std::printf("Unknown Error Occured!");
        return_value = 3;
    }
    
    return return_value;

}