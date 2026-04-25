// ======================
// Pines del Motor Driver TB6612FNG (Según tu imagen)
// ======================
// Canal A
#define PWMA_PIN 32   // Etiqueta pwmA (Pin 6)
#define AIN1_PIN 14   // Etiqueta A1 (Pin 11)
#define AIN2_PIN 27   // Etiqueta A2 (Pin 10)

// Canal B (Si vas a usar el segundo canal)
#define PWMB_PIN 25   // Etiqueta pwmB (Pin 8)
#define BIN1_PIN 26   // Etiqueta S1 (Pin 26)
#define BIN2_PIN 13   // Etiqueta S7 (Pin 13) - Ajustado según tu diagrama

// Otros pines del driver
#define STBY_PIN 12   // Etiqueta 3V3 del Driver conectado al Pin 12 (nSTBY)

// Pines del Encoder (Lectura)
#define ENCA 34       // Etiqueta encA_3V3 (Pin 4)
#define ENCB 35       // Etiqueta encB_3V3 (Pin 5)

// ======================
// Configuración PWM (ESP32 LEDC)
// ======================
const int frecuencia = 5000;
const int resolucion = 8; // 0-255

void setup() {
  Serial.begin(115200);

  // Configuración de pines de control
  pinMode(AIN1_PIN, OUTPUT);
  pinMode(AIN2_PIN, OUTPUT);
  pinMode(BIN1_PIN, OUTPUT);
  pinMode(BIN2_PIN, OUTPUT);
  pinMode(STBY_PIN, OUTPUT);
  
  // Pines del Encoder como entrada
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);

  // Salimos de Standby para que el driver funcione
  digitalWrite(STBY_PIN, HIGH);

  // Configuración de PWM en el ESP32
  ledcAttach(PWMA_PIN, frecuencia, resolucion);
  ledcAttach(PWMB_PIN, frecuencia, resolucion);

  Serial.println("Driver TB6612FNG Listo.");
  Serial.println("Ingresa valor de velocidad (-255 a 255):");
}

void loop() {
  if (Serial.available()) {
    // Leemos el valor de velocidad directamente del Serial
    int velocidad = Serial.parseInt(); 
    
    // Limpiar el buffer
    while(Serial.available()) Serial.read();

    controlMotores(velocidad);
    
    Serial.print("Velocidad aplicada: ");
    Serial.println(velocidad);
  }
}

// Función para controlar ambos motores en simultáneo
void controlMotores(int pwm) {
  int absPWM = constrain(abs(pwm), 0, 255);

  if (pwm > 0) {
    // Adelante
    digitalWrite(AIN1_PIN, HIGH);
    digitalWrite(AIN2_PIN, LOW);
    digitalWrite(BIN1_PIN, HIGH);
    digitalWrite(BIN2_PIN, LOW);
  } 
  else if (pwm < 0) {
    // Reversa
    digitalWrite(AIN1_PIN, LOW);
    digitalWrite(AIN2_PIN, HIGH);
    digitalWrite(BIN1_PIN, LOW);
    digitalWrite(BIN2_PIN, HIGH);
  } 
  else {
    // Parada (Freno)
    digitalWrite(AIN1_PIN, LOW);
    digitalWrite(AIN2_PIN, LOW);
    digitalWrite(BIN1_PIN, LOW);
    digitalWrite(BIN2_PIN, LOW);
  }

  // Aplicar potencia
  ledcWrite(PWMA_PIN, absPWM);
  ledcWrite(PWMB_PIN, absPWM);
}