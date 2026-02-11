# MiniQhali Firmware - ESP32 Node

Este directorio contiene el firmware desarrollado para el nodo de adquisición de signos vitales del proyecto **MiniQhali**, basado en el microcontrolador **ESP32 (Dev Module)**.

## 🚀 Descripción General

El firmware está diseñado para capturar datos de oximetría ($SpO_2$), frecuencia cardíaca (BPM) y temperatura corporal mediante los sensores **MAX30102** y **MLX90614**. 

Una característica clave es su **arquitectura dual**, que permite alternar entre una transmisión de datos por **Puerto Serial (JSON)** o mediante el protocolo **MQTT** sobre Wi-Fi, facilitando la integración con una Raspberry Pi 5 o un broker externo.

---

## 🛠 Configuración de Flags y Modos

Para simplificar el mantenimiento, el código utiliza directivas de preprocesador (`#define`). No necesitas subir dos códigos distintos; solo debes comentar o descomentar una línea antes de cargar el programa:

### 1. Modo MQTT (IoT)
Para enviar datos de forma inalámbrica a la Raspberry Pi 5:
* Asegúrate de que la línea ` #define MODO_MQTT` **no** esté comentada.
* El ESP32 intentará conectarse al SSID configurado y publicará en el tópico `miniqhali/data`.

### 2. Modo Serial (USB)
Para enviar datos por cable directamente a un script de Python o consola:
* Comenta la línea: `//#define MODO_MQTT`.
* El firmware omitirá toda la pila de red (Wi-Fi/MQTT), ahorrando memoria y tiempo de ejecución.

---

## ⚙️  Parámetros a Modificar

Antes de compilar, asegúrate de ajustar las siguientes variables según tu entorno de red en la sección de configuración del código:

| Variable | Descripción |
| :--- | :--- |
| `ssid` | Nombre de tu red Wi-Fi (ej. tu hotspot o router). |
| `password` | Contraseña de la red Wi-Fi. |
| `mqtt_server` | Dirección IP de tu Raspberry Pi 5 (donde corre Mosquitto). |
| `mqtt_topic` | El canal donde se publicarán los JSON. |

---

## 📦 Estructura de Datos (JSON)

Independientemente del modo elegido, el dispositivo entrega un objeto JSON estructurado para facilitar el parseo en el backend:

```json
{
  "timestamp": 12540,
  "heartRate": 75,
  "spo2": 98,
  "finger_detected": true,
  "tempObject": 36.5,
  "tempAmbient": 24.2
}
```

## 🔧 Requisitos Técnicos
Hardware: ESP32 Dev Module.

Sensores: * MAX30102 (I2C)

MLX90614 (I2C)

Librerías Necesarias (Arduino IDE):

Adafruit_MLX90614

SparkFun_MAX3010x

ArduinoJson

PubSubClient (solo para modo MQTT)

## 📂 Organización de Archivos
main/: Código principal unificado con soporte dual.

tests/: Scripts de diagnóstico (I2C Scanner y Servo Test) para validación de hardware.
