#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include "MAX30105.h"
#include "spo2_algorithm.h"
#include <ArduinoJson.h>

// ==========================================
// CONFIGURACIÓN DE MODO
// ==========================================
#define MODO_MQTT  // Comenta esta línea para usar solo SERIAL

#ifdef MODO_MQTT
  #include <WiFi.h>
  #include <PubSubClient.h>

  const char* ssid = "Legion-Slim-5";
  const char* password = "12345678";
  const char* mqtt_server = "192.168.0.71";
  const char* mqtt_topic = "miniqhali/data";

  WiFiClient espClient;
  PubSubClient client(espClient);
#endif

// Objetos de sensores
MAX30105 particleSensor;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

// Buffers y variables globales
const int32_t bufferLength = 100; 
uint32_t irBuffer[bufferLength]; 
uint32_t redBuffer[bufferLength]; 

int32_t spo2;
int8_t validSPO2;
int32_t heartRate;
int8_t validHeartRate;

// ==========================================
// FUNCIONES AUXILIARES
// ==========================================
#ifdef MODO_MQTT
void setup_wifi() {
  delay(10);
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado. IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    String clientId = "ESP32-MiniQhali-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("conectado");
    } else {
      Serial.print("falló, rc=");
      Serial.print(client.state());
      Serial.println(" reintentando en 5s");
      delay(5000);
    }
  }
}
#endif

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(115200);
  Wire.begin(); 

  #ifdef MODO_MQTT
    setup_wifi();
    client.setServer(mqtt_server, 1883);
  #endif

  // Inicializar MAX30102
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) { 
    Serial.println("Error: MAX30102 no encontrado.");
    while (1);
  }

  particleSensor.setup(60, 1, 2, 100, 411, 4096);

  // Inicializar MLX90614
  if (!mlx.begin()) {
    Serial.println("Error: MLX90614 no encontrado.");
    while (1);
  }

  #ifdef MODO_MQTT
    Serial.println("Sistema iniciado (Modo MQTT).");
  #else
    Serial.println("Sistema iniciado (Modo Serial).");
  #endif
}

// ==========================================
// LOOP PRINCIPAL
// ==========================================
void loop() {
  #ifdef MODO_MQTT
    if (!client.connected()) reconnect();
    client.loop();
  #endif

  // FASE 1: Recolección (1 segundo)
  for (byte i = 0; i < bufferLength; i++) {
    while (particleSensor.available() == false) particleSensor.check(); 
    redBuffer[i] = particleSensor.getRed();
    irBuffer[i]  = particleSensor.getIR();
    particleSensor.nextSample(); 
  }

  // FASE 2: Cálculos y Detección de Dedo
  long irAverage = 0;
  for (byte i = 0; i < bufferLength; i++) irAverage += irBuffer[i];
  irAverage /= bufferLength;

  bool hayDedo = (irAverage > 50000); 

  if (hayDedo) {
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  } else {
    spo2 = 0; heartRate = 0;
  }

  // FASE 3: Temperaturas
  float tempObject = mlx.readObjectTempC();
  float tempAmbient = mlx.readAmbientTempC();

  if (isnan(tempObject)) tempObject = 0.0;
  if (isnan(tempAmbient)) tempAmbient = 0.0;

  if (tempObject > 30.0) tempObject += 4.0;

  // FASE 4: Lógica de Envío
  StaticJsonDocument<256> doc;
  doc["timestamp"] = millis();
  bool enviarDatos = false;

  if (hayDedo && validHeartRate == 1 && validSPO2 == 1 && heartRate > 40) {
    doc["heartRate"] = heartRate;
    doc["spo2"] = spo2;
    doc["finger_detected"] = true;
    enviarDatos = true;
  } else if (!hayDedo) {
    doc["heartRate"] = 0;
    doc["spo2"] = 0;
    doc["finger_detected"] = false;
    enviarDatos = true; 
  }

  if (enviarDatos) {
    doc["tempObject"] = tempObject;
    doc["tempAmbient"] = tempAmbient;
    
    #ifdef MODO_MQTT
      char jsonBuffer[512];
      serializeJson(doc, jsonBuffer);
      client.publish(mqtt_topic, jsonBuffer);
      Serial.print("MQTT Publicado: ");
      Serial.println(jsonBuffer);
    #else
      serializeJson(doc, Serial);
      Serial.println();
    #endif
  }
}