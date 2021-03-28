#include <Arduino.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>

// Thanks to Jeff Rowberg's example code for the MPU6050 DMP6

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2012 Jeff Rowberg

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
===============================================
*/

MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

Quaternion q;           // [w, x, y, z]         quaternion container

void dataReadyISR(){
    dmpReady = true;
}

void waitTillProceedSignal(){
    while(Serial.available() && Serial.read());
    int proceed = 0;
    do{
        while(!Serial.available());
        proceed = Serial.read();
    }while(!proceed);
}

int initializeEverything(){

    int retVal = -1;

    mpu.initialize();
    if(!mpu.testConnection()) return retVal;
    else retVal--;

    devStatus = mpu.dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    if(devStatus != 0) return retVal;
    else retVal--;

    mpu.setDMPEnabled(true);

    attachInterrupt(digitalPinToInterrupt(2), dataReadyISR, RISING);

    // get expected DMP packet size for later comparison
    packetSize = mpu.dmpGetFIFOPacketSize();

    return 0;

}

void setup(){

    Serial.begin(115200, SERIAL_8E1);
    
    Wire.begin();
    Wire.setClock(400000);

    int err = initializeEverything();

    while(!Serial.availableForWrite());
    Serial.write((const char*)&err, 1);
}

void loop(){

    if(dmpReady || fifoCount > packetSize){

        dmpReady = false;

        mpuIntStatus = mpu.getIntStatus();
        fifoCount = mpu.getFIFOCount();

        // check for overflow
        if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
            // reset so we can continue cleanly
            mpu.resetFIFO();

            // Serial.println("FIFO Overflow!");

        // Check for DMP data ready interrup
        }else if((mpuIntStatus & 0x02) > 0){

            // wait for correct available data length, should be a VERY short wait
            while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

            // read a packet from FIFO
            mpu.getFIFOBytes(fifoBuffer, packetSize);

            // track FIFO count here in case there is > 1 packet available
            // (this lets us immediately read more without waiting for an interrupt)
            fifoCount -= packetSize;

            mpu.dmpGetQuaternion(&q, fifoBuffer);

        }
    }

    if(Serial.available()){
        do Serial.read();
        while(Serial.available());
        while(!Serial.availableForWrite());
        Serial.write((const char*)&q, sizeof(q));
    }
}