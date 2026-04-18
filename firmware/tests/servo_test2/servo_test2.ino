#include <ESP32Servo.h>

// --- Configuración de Pines ---
// Los pines se asignan según el esquema ESP32 DEVKIT
// Asegúrate de que los pines S1 a S6 corresponden a los GPIO correctos
// y están conectados a la línea de señal de los servos.

#define PIN_S1  4  // Pin D4 / GPIO4     
#define PIN_S2 16  // Pin D16 / UART2_RX
#define PIN_S3 17  // Pin D17 / UART2_TX
#define PIN_S4 18  // Pin D18
#define PIN_S5 19  // Pin D19
#define PIN_S6 23  // Pin D23
#define PIN_S7 13 // Pin D13

// --- Tipos de Servo (por información) ---
// S1-S4: MG996R (Grandes)
// S5-S6: MG90S (Pequeños)

Servo servo1; // Servo de mano fija. Inicial en 90 grados. Rango de 0 a 180 (Apuntando hacia la izquierda viendo a MiniQHali de frente, apuntando a la derecha viendo a MiniQhali de frente)
Servo servo2; // Servo del cuello (asentir) Inicial 90 grados. Rango de 60 a 110 (Abajo hacia arriba)
Servo servo3; // Servo del cuello (negación) Inicial 90 grados. Rango de 50 a 130 (Izquierda a derecha)
Servo servo4; // Servo del hombro (Extención) Inicial 90 graos. Rango de 0 a 180 (Hacia atrás, hacia el frente)
Servo servo5; // Servo del hombro (Abducción) Inicial 90 grados. Rango 30 - 180 (hacia arriba, hacia abajo)
Servo servo6; // Servo de la muñeca móvil. Inicial 90 grados. Rango 0 a 180 grados
Servo servo7; // Servo de los dedos de brazo móvil. Por determinar

Servo servos[] = {servo1, servo2, servo3, servo4, servo5, servo6};
int servoPins[] = {PIN_S1, PIN_S2, PIN_S3, PIN_S4, PIN_S5, PIN_S6};
const char* servoNames[] = {"S1 (MG996R)", "S2 (MG996R)", "S3 (MG996R)", "S4 (MG996R)", "S5 (MG90S)", "S6 (MG90S)"};

int currentServoIndex = -1; // -1 significa que no hay servo seleccionado

void setup() {
  Serial.begin(115200);
  
  // No inicializamos los servos en el setup para que no se muevan al encender
  // y para asegurar un arranque más limpio. Los inicializaremos bajo demanda.

  Serial.println("\n--- Control de Pruebas de Servomotores ESP32 ---");
  Serial.println("Selecciona un servo (1-6) escribiendo su número.");
  Serial.println("Luego, ingresa un ángulo entre 0 y 180.");
  mostrarMenu();
}

void loop() {
  if (Serial.available() > 0) {
    if (currentServoIndex == -1) {
      // Estamos esperando la selección del servo
      char incoming = Serial.read();
      if (isdigit(incoming)) {
        int selection = incoming - '0';
        if (selection >= 1 && selection <= 6) {
          currentServoIndex = selection - 1;
          
          // Adjuntar el servo seleccionado si no está ya adjuntado
          // Esto configura el pin como salida PWM y empieza a enviar señal.
          if (!servos[currentServoIndex].attached()) {
             // La librería ESP32Servo automáticamente maneja los temporizadores PWM.
             // Para servos estándar de 180°, no se necesitan más argumentos.
             servos[currentServoIndex].attach(servoPins[currentServoIndex]);
             Serial.print("--- Servo ");
             Serial.print(servoNames[currentServoIndex]);
             Serial.println(" ACTIVADO y listo para moverse. ---");
          } else {
             Serial.print("--- Servo ");
             Serial.print(servoNames[currentServoIndex]);
             Serial.println(" re-seleccionado. ---");
          }

          Serial.println("Ahora, ingresa un ángulo (0-180):");
        } else {
          Serial.println("Selección inválida. Por favor, ingresa un número del 1 al 6.");
          mostrarMenu();
        }
      }
    } else {
      // Estamos esperando el ángulo del servo seleccionado
      String input = Serial.readStringUntil('\n');
      input.trim(); // Limpiar espacios y retornos de carro

      if (input.equalsIgnoreCase("c") || input.equalsIgnoreCase("cancel")) {
        // Opción para deseleccionar el servo y volver al menú principal
        Serial.print("--- Deseleccionando Servo ");
        Serial.print(servoNames[currentServoIndex]);
        Serial.println(" y deteniendo su PWM. ---");
        // detach() libera el recurso PWM y detiene el tren de pulsos.
        servos[currentServoIndex].detach();
        currentServoIndex = -1;
        mostrarMenu();
      } else {
        // Intentar leer el ángulo
        int angle = input.toInt();
        if (angle >= 0 && angle <= 180) {
          Serial.print("Moviendo Servo ");
          Serial.print(servoNames[currentServoIndex]);
          Serial.print(" a ");
          Serial.print(angle);
          Serial.println(" grados...");
          
          // Escribir el ángulo al servo.
          servos[currentServoIndex].write(angle);
          
          // Después de moverlo, volvemos a preguntar por el ángulo para este servo.
          Serial.print("\nIngrese otro ángulo (0-180) para el ");
          Serial.print(servoNames[currentServoIndex]);
          Serial.println(" o escribe 'c' para cancelar la selección:");
        } else {
          Serial.println("Ángulo inválido. Ingrese un número entre 0 y 180.");
        }
      }
    }
    
    // Limpiar cualquier residuo en el búfer
    while (Serial.available() > 0) Serial.read();
  }
}

void mostrarMenu() {
  Serial.println("\n--- MENÚ PRINCIPAL ---");
  for (int i = 0; i < 6; i++) {
    Serial.print(i + 1);
    Serial.print(". ");
    Serial.println(servoNames[i]);
  }
  Serial.println("--- Ingrese el número del servo a probar ---");
}