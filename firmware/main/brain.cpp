#include "brain.h"

Brain::Brain(Robot& r) : robot(r), estadoActual(IDLE) {
    // Posición inicial neutra
    for(int i = 0; i < 7; i++) angulosActuales[i] = 90; 
}

void Brain::procesarEntrada() {
    if (Serial.available() > 0) {
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, Serial);

        if (error) {
            while(Serial.available() > 0) Serial.read(); // Limpiar basura
            return;
        }

        // Si el JSON dice "action": "measure", pasamos a medir
        if (doc.containsKey("action") && doc["action"] == "measure") {
            estadoActual = MEASURING;
        } 
        // Si el JSON trae "angles", actualizamos la posición y volvemos a IDLE
        else if (doc.containsKey("angles")) {
            JsonArray arr = doc["angles"];
            for (int i = 0; i < arr.size() && i < 7; i++) {
                angulosActuales[i] = arr[i];
            }
            estadoActual = IDLE; 
        }
    }
}

void Brain::actualizar() {
    switch (estadoActual) {
        case IDLE:
            robot.mover(angulosActuales);
            break;

        case MEASURING:
            // Ejecutar la rutina de salud
            robot.realizarMediciones();
            // Al terminar, el robot queda listo para moverse de nuevo
            estadoActual = IDLE;
            break;
    }
}