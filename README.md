# MotionCube #

![](https://raw.githubusercontent.com/swarnavaghosh04/MotionCube/res/MotionCube.gif)

## Dependencies

- [Jeff Rowberg's digital motion processing program for MPU6050](https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050)
- [My OpenGL C++ Library "GLider"](https://github.com/swarnavaghosh04/GLider)
- [The ASIO Library (non boost) for reading COM Port](https://think-async.com/Asio/)
- [SDL2 for cross platform window managment](https://www.libsdl.org/)

## Installation

1. Install GLM, SDL2, Asio (non-Boost), and Python-pip:

```
# Archlinux:
sudo pacman -Su glm sdl2 asio python-pip cmake ninja-build

# Debian:
sudo apt-get libglm-dev libsdl2-dev libasio-dev python-pip cmake ninja
```

2. Install PlatformIO CLI Core (if you do not have PlatformIO IDE):

```
pip install platformio
```

3. Connect Arduino to computer

4. Build and upload to Arduino:

```
cd MotionCubeArduino
pio run
cd ..
```

5. Build and install desktop application:

```
cd MotionCubeDesktop
mkdir build && cd build
cmake ..
cmake --build .
cmake --install . --prefix "$HOME/MotionCube"
```

6. Run desktop application with serial port as argument:

```
cd $HOME/MotionCube/bin
./MotionCube ARDUINO_SERIAL_PORT

# Windows Example
MotionCube COM3

# Linux Example
./MotionCube /dev/ttyACM0
```
