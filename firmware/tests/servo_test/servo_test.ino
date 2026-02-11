#include <ESP32Servo.h>

Servo miServo;

// Variable para saber si ya tenemos un pin configurado
// -1 indica que no se ha seleccionado pin todavía
int pinActual = -1;

void setup() {
  Serial.begin(115200);
  delay(1000); // Esperar a que el monitor serial arranque
  
  // Configuración de frecuencia para MG996R
  miServo.setPeriodHertz(50); 
  
  Serial.println("--- Sistema Iniciado ---");
  Serial.println("Por favor, ingresa el numero del PIN GPIO a utilizar:");
}

void loop() {
  // Solo procesamos si hay datos en el monitor serial
  if (Serial.available() > 0) {
    
    // Leemos todo lo que escribió el usuario hasta el "Enter" (\n)
    String entrada = Serial.readStringUntil('\n');
    entrada.trim(); // Elimina espacios en blanco sobrantes

    // --- MODO CONFIGURACIÓN DE PIN ---
    if (pinActual == -1) {
      // Intentar convertir la entrada a número
      int pinIngresado = entrada.toInt();

      // Validación simple: GPIO debe ser positivo y menor a 40 (aprox en ESP32)
      // Nota: entrada.toInt() devuelve 0 si no es numero, cuidado si usas el pin 0.
      if (pinIngresado >= 0 && pinIngresado < 40) {
        pinActual = pinIngresado;
        
        // Conectar el servo al nuevo pin
        // Usamos 500-2500us para el MG996R
        miServo.attach(pinActual, 500, 2500);
        
        Serial.print(">> Pin configurado en GPIO: ");
        Serial.println(pinActual);
        Serial.println("------------------------------------------------");
        Serial.println("AHORA: Ingresa el angulo (0-180)");
        Serial.println("O escribe 'C' para cambiar de Pin.");
      } else {
        Serial.println("Pin no válido. Intenta de nuevo (Ej: 4, 13, 2):");
      }
    }
    
    // --- MODO CONTROL DE ÁNGULO ---
    else {
      // Verificamos si el usuario quiere cambiar de pin (escribiendo 'C' o 'c')
      if (entrada.equalsIgnoreCase("C")) {
        Serial.println("\nRegresando a configuracion de pin...");
        
        // ¡IMPORTANTE! Desconectar el servo del pin actual antes de cambiar
        miServo.detach(); 
        
        pinActual = -1; // Volvemos al estado de "sin pin"
        Serial.println("Ingresa el numero del PIN GPIO a utilizar:");
      }
      // Si no es 'C', asumimos que es un ángulo
      else {
        int angulo = entrada.toInt();

        if (angulo >= 0 && angulo <= 180) {
          Serial.print("Moviendo servo en Pin ");
          Serial.print(pinActual);
          Serial.print(" a: ");
          Serial.print(angulo);
          Serial.println("°");
          
          miServo.write(angulo);
        } else {
          Serial.println("Error: El angulo debe ser entre 0 y 180. (O 'C' para salir)");
        }
      }
    }
  }
}