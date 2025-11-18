#include "BluetoothSerial.h"
#include "DHT.h"
#include <Ticker.h>

// ==== Pines ====
#define DHTPIN 4
#define DHTTYPE DHT11
#define FANPIN 5
#define LEDPIN 26        // LED controlado por Bluetooth
#define LDRPIN 34        // Entrada analógica del LDR
#define LED_LUZ 27       // LED controlado por el LDR

// Sensor ultrasónico
#define TRIG_PIN 12
#define ECHO_PIN 14
#define LED_ULTRA 25     // LED que indica detección por ultrasonido

// ==== Objetos ====
BluetoothSerial SerialBT;
DHT dht(DHTPIN, DHTTYPE);
Ticker tickerDHT;
Ticker tickerLuz;

// ==== Variables ====
volatile bool leerTemperatura = false;
volatile bool leerLuz = false;
String device_name = "ESP32-MULTISENSOR";

// --- Funciones de los timers ---
void activarLecturaDHT() { leerTemperatura = true; }
void activarLecturaLuz() { leerLuz = true; }

void setup() {
  Serial.begin(115200);
  SerialBT.begin(device_name);
  dht.begin();

  pinMode(FANPIN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);
  pinMode(LED_LUZ, OUTPUT);
  pinMode(LDRPIN, INPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_ULTRA, OUTPUT);

  Serial.printf("Bluetooth \"%s\" iniciado.\nEmpareja el dispositivo desde tu celular.\n", device_name.c_str());

  // Timers
  tickerDHT.attach(2, activarLecturaDHT);  // cada 2 s lee DHT
  tickerLuz.attach(1, activarLecturaLuz);  // cada 1 s lee LDR
}

void loop() {
  // --- Bluetooth ---
  if (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.write(c);

    switch (c) {
      case 'A':
        digitalWrite(LEDPIN, HIGH);
        SerialBT.println("LED encendido manualmente.");
        break;

      case 'B':
        digitalWrite(LEDPIN, LOW);
        delay(200);
        digitalWrite(LEDPIN, HIGH);
        SerialBT.println("LED parpadeó.");
        break;
    }
  }

  // --- Lectura DHT ---
  if (leerTemperatura) {
    leerTemperatura = false;
    float t = dht.readTemperature();

    if (isnan(t)) {
      Serial.println("Error al leer el sensor DHT!");
      SerialBT.println("Error al leer el sensor DHT!");
    } else {
      // Control ventilador
      if (t > 28)
        digitalWrite(FANPIN, HIGH);
      else
        digitalWrite(FANPIN, LOW);

      Serial.printf("Temperatura: %.1f °C\n", t);
      SerialBT.printf("Temperatura: %.1f °C\n", t);
    }
  }

  // --- Lectura LDR ---
  if (leerLuz) {
    leerLuz = false;
    int valorLuz = analogRead(LDRPIN);

    Serial.print("Valor LDR: ");
    Serial.println(valorLuz);

    if (valorLuz > 800)  // Mucha luz
      digitalWrite(LED_LUZ, LOW);
    else                 // Poca luz
      digitalWrite(LED_LUZ, HIGH);
  }

  // --- Sensor ultrasónico ---
  long duration;
  float distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000);
  distance = duration * 0.034 / 2;

  if (distance > 0 && distance <= 120) {
    digitalWrite(LED_ULTRA, HIGH);
    Serial.println("⚠️ Objeto detectado por ultrasonido!");
    SerialBT.println("⚠️ Objeto detectado por ultrasonido!");
  } else {
    digitalWrite(LED_ULTRA, LOW);
  }

  delay(200);
}
