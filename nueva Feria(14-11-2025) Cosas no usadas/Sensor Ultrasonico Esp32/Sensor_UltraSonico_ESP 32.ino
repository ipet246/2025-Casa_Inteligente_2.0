#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

#define BUZZER_PIN 26
#define LED_ULTRA 27
#define TRIG_PIN 13
#define ECHO_PIN 14

long duracion;
float distancia;
const int DISTANCIA_UMBRAL = 30;  // cm

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Ultrasonico"); // Nombre del Bluetooth

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_ULTRA, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_ULTRA, LOW);

  Serial.println("Prueba del sensor HC-SR04 + Bluetooth lista");
}

void loop() {
  // --- Medición del sensor ultrasónico ---
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duracion = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duracion == 0) {
    Serial.println("⚠️ No hay señal de retorno");
    delay(500);
    return;
  }

  distancia = duracion * 0.034 / 2;
  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  // --- Control por Bluetooth ---
  if (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.print("Comando recibido: ");
    Serial.println(c);

    switch (c) {
      case 'C':  // Detectar con LED
        if (distancia < DISTANCIA_UMBRAL) {
          digitalWrite(LED_ULTRA, HIGH);
        } else {
          digitalWrite(LED_ULTRA, LOW);
        }
        break;

      case 'D':  // Detectar con buzzer
        if (distancia < DISTANCIA_UMBRAL) {
          digitalWrite(BUZZER_PIN, HIGH);
        } else {
          digitalWrite(BUZZER_PIN, LOW);
        }
        break;
    }
  }

  delay(300);
}
