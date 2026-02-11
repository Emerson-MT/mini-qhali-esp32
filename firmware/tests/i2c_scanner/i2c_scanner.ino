#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial); // Esperar a que el monitor serie abra
  
  Serial.println("\nI2C Scanner");

  // OPCIÓN A: Usar pines por defecto (D21 = SDA, D22 = SCL)
  //Wire.begin();
  
  // OPCIÓN B: Si quisieras probar OTROS pines, descomenta y edita esta línea:
  Wire.begin(21, 22); // Wire.begin(SDA, SCL);
}

void loop() {
  byte error, address;
  int nDevices;

  Serial.println("Escaneando...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    // Wire.beginTransmission inicia la comunicación con la dirección dada
    Wire.beginTransmission(address);
    
    // Wire.endTransmission envía los datos y devuelve el estado
    // 0 = Éxito (Dispositivo encontrado)
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Dispositivo I2C encontrado en la dirección 0x");
      if (address < 16) 
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");

      nDevices++;
    }
    else if (error == 4) {
      Serial.print("Error desconocido en la dirección 0x");
      if (address < 16) 
        Serial.print("0");
      Serial.println(address, HEX);
    }    
  }
  
  if (nDevices == 0)
    Serial.println("No se encontraron dispositivos I2C\n");
  else
    Serial.println("Escaneo completado\n");

  delay(5000); // Esperar 5 segundos para el siguiente escaneo
}