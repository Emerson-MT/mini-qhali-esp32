#include "robot.h"
#include "brain.h"

// Creamos las instancias de las clases
Robot miniQhali;
Brain cerebro(miniQhali);

void setup() {
    Serial.begin(115200);
    
    // Inicializar el hardware (sensores y configuración I2C)
    miniQhali.begin();
    
    Serial.println("{\"system\": \"MiniQhali Online\"}");
}

void loop() {
    // 1. Escuchar a la Raspberry Pi
    cerebro.procesarEntrada();
    
    // 2. Mover servos o Medir biometría según el estado
    cerebro.actualizar();
    
    // Pequeña pausa para no saturar el procesador
    delay(15);
}