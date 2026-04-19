#include "robot.h"

Robot::Robot() : mlx() {}

void Robot::begin() {
    Wire.begin();
    
    // Inicializar MAX30102
    if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
        Serial.println("{\"error\": \"MAX30102 no encontrado\"}");
    }
    // Configuración original de tu main.ino
    particleSensor.setup(60, 1, 2, 100, 411, 4096);

    // Inicializar MLX90614
    if (!mlx.begin()) {
        Serial.println("{\"error\": \"MLX90614 no encontrado\"}");
    }
}

void Robot::mover(int angulos[7]) {
    for (int i = 0; i < 7; i++) {
        int target = angulos[i];

        // Restricciones de seguridad de tus comentarios en servo_test2.ino
        if (i == 1) target = constrain(target, 60, 110);  // S2: Cuello (asentir)
        if (i == 2) target = constrain(target, 50, 130);  // S3: Cuello (negación)
        if (i == 4) target = constrain(target, 30, 180);  // S5: Hombro (abducción)

        if (!servos[i].attached()) {
            servos[i].attach(pins[i]);
        }
        servos[i].write(target);
    }
}

void Robot::realizarMediciones() {
    int32_t spo2, heartRate;
    int8_t validSPO2, validHeartRate;

    // FASE 1: Recolección de muestras (main.ino)
    for (byte i = 0; i < bufferLength; i++) {
        while (particleSensor.available() == false) particleSensor.check();
        redBuffer[i] = particleSensor.getRed();
        irBuffer[i] = particleSensor.getIR();
        particleSensor.nextSample(); 
    }

    // FASE 2: Cálculos y detección de dedo
    long irAverage = 0;
    for (byte i = 0; i < bufferLength; i++) irAverage += irBuffer[i];
    irAverage /= bufferLength;

    if (irAverage > 50000) {
        maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
    } else {
        spo2 = 0; heartRate = 0; validSPO2 = 0; validHeartRate = 0;
    }

    // FASE 3: Temperaturas
    float tempObject = mlx.readObjectTempC();
    if (!isnan(tempObject) && tempObject > 30.0) tempObject += 4.0;

    // Enviar datos formateados a la Raspberry Pi
    StaticJsonDocument<256> doc;
    doc["heartRate"] = (validHeartRate == 1) ? heartRate : 0;
    doc["spo2"] = (validSPO2 == 1) ? spo2 : 0;
    doc["temp"] = isnan(tempObject) ? 0.0 : tempObject;
    doc["finger_detected"] = (irAverage > 50000);
    
    serializeJson(doc, Serial);
    Serial.println();
}