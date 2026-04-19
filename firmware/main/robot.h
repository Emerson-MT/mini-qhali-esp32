#ifndef ROBOT_H
#define ROBOT_H

#include <ESP32Servo.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <ArduinoJson.h>

class Robot {
private:
    // Servos y Pines
    Servo servos[7];
    const int pins[7] = {4, 16, 17, 18, 19, 23, 13}; // S1 a S7
    
    // Sensores
    MAX30105 particleSensor;
    Adafruit_MLX90614 mlx;

    // Buffers para Biometría
    static const int32_t bufferLength = 100; 
    uint32_t irBuffer[bufferLength];
    uint32_t redBuffer[bufferLength];

public:
    Robot();
    void begin();
    void mover(int angulos[7]);
    void realizarMediciones();
};

#endif
