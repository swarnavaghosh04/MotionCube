#include <asio.hpp>
#include <cstdio>
#include <thread>
#include <chrono>
#include <atomic>

std::atomic<bool> keepRunning = true;
asio::io_service io;
asio::serial_port serial(io);
struct{
    float w,x,y,z; // arduino data format
} gyro;
auto gyro_buf = asio::buffer(&gyro, sizeof(gyro));

void exitListener(){
    std::fgetc(stdin);
    keepRunning = false;
    return;
}

int setupSerialPort(const char* port){

    serial.open(port);
    if(!serial.is_open()){
        std::puts("Serial cannot be opened");
        return 1;
    }

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
    return 0;
}

int getDMPInitCode(){
    char c;
    auto buf = asio::buffer(&c, 1);
    std::puts("Trying to contact...");
    asio::read(serial, buf);
    while(c != 0){
        std::putchar(c);
        asio::read(serial, buf);
    }
    asio::read(serial, buf);
    std::printf("MPU DMP Initialized with code %d\n", (int)c);
    return c;
}

int main(int argc, const char* argv[]){

    if(argc < 2){
        std::puts("Please enter serial port");
        return 1;
    }

    if(setupSerialPort(argv[1]) != 0) return 2;

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if(getDMPInitCode() != 0){
        serial.close();
        return 3;
    }

    std::puts("Press enter key to terminate");
    std::thread ender(exitListener);

    char c = 1;
    auto buf = asio::buffer(&c, 1);

    while(keepRunning){
        asio::write(serial, buf);
        asio::read(serial, gyro_buf);
        std::printf("w: %.2f, x: %.2f, y: %.2f, z: %.2f\n", gyro.w, gyro.x, gyro.y, gyro.z);
    }

    serial.close();

    ender.join();

    return 0;
}