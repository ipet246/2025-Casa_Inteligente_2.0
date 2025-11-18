#include "BluetoothSerial.h"
#include "DHT.h"
#include <Ticker.h>

// ==== Pines ====
#define DHTPIN 4           // Sensor de temperatura
#define DHTTYPE DHT11
#define FANPIN 5           // Ventilador
#define LED_BT 26          // LED controlado por Bluetooth
#define LDRPIN 33          // Sensor LDR (entrada analógica)
#define LED_LUZ 27         // LED controlado por luz
#define PIR_PIN 13         // ⚠️ PIR en GPIO 13 (no uses GPIO1)
#define LED_PIR 2          // LED de detección de movimiento
#define BUZZER 19

int vigilante = 0;

// ==== Objetos ====
BluetoothSerial SerialBT;
DHT dht(DHTPIN, DHTTYPE);
Ticker tickerDHT;
Ticker tickerLuz;

// ==== Variables ====
volatile bool leerTemperatura = false;
volatile bool leerLuz = false;
String device_name = "ESP32-COMPLETO";

// ==== Funciones de los timers ====
void activarLecturaDHT() { leerTemperatura = true; }
void activarLecturaLuz() { leerLuz = true; }

// ==== Filtro de estabilidad del PIR ====
bool leerPIREstable(int pin) {
  int cuentaAlta = 0;
  for (int i = 0; i < 10; i++) {
    if (digitalRead(pin) == HIGH) cuentaAlta++;
    delay(10);
  }
  return (cuentaAlta > 7); // Solo si 8 de 10 lecturas son HIGH
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin(device_name);
  dht.begin();

  pinMode(FANPIN, OUTPUT);
  pinMode(LED_BT, OUTPUT);
  pinMode(LED_LUZ, OUTPUT);
  pinMode(LDRPIN, INPUT);
  pinMode(PIR_PIN, INPUT); // Si tenés una resistencia externa, usá INPUT_PULLDOWN
  pinMode(LED_PIR, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  Serial.printf("Bluetooth \"%s\" iniciado.\nEmpareja el dispositivo desde tu celular.\n", device_name.c_str());

  // Timers
  tickerDHT.attach(2, activarLecturaDHT);  // cada 2 s lee DHT
  tickerLuz.attach(1, activarLecturaLuz);  // cada 1 s lee LDR
}

void loop() {
  // === Sensor PIR con filtro ===
  bool pirValor = leerPIREstable(PIR_PIN);

  if (vigilante == 0) {
    if (pirValor) {
      digitalWrite(LED_PIR, HIGH);
      Serial.println("Movimiento detectado!");
      SerialBT.println("Movimiento detectado!");
    } else {
      digitalWrite(LED_PIR, LOW);
    }
  } else {
    if (pirValor) {
      digitalWrite(LED_PIR, HIGH);
      digitalWrite(BUZZER, HIGH);
      Serial.println("Movimiento detectado con alarma!");
      SerialBT.println("Movimiento detectado con alarma!");
    } else {
      digitalWrite(LED_PIR, LOW);
      digitalWrite(BUZZER, LOW);
    }
  }

  // === Bluetooth ===
  if (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.write(c);

    switch (c) {
      case 'A':
        digitalWrite(LED_BT, HIGH);
        SerialBT.println("LED encendido manualmente.");
        break;

      case 'B':
        digitalWrite(LED_BT, LOW);
        delay(200);
        digitalWrite(LED_BT, HIGH);
        SerialBT.println("LED parpadeó.");
        break;

      case 'C':
        vigilante = !vigilante; // alterna entre 0 y 1
        SerialBT.printf("Modo vigilante: %s\n", vigilante ? "Activado" : "Desactivado");
        break;
    }
  }

  // === Lectura DHT ===
  if (leerTemperatura) {
    leerTemperatura = false;
    float t = dht.readTemperature();

    if (isnan(t)) {
      Serial.println("Error al leer el sensor DHT!");
      SerialBT.println("Error al leer el sensor DHT!");
    } else {
      if (t > 28)
        digitalWrite(FANPIN, HIGH);
      else
        digitalWrite(FANPIN, LOW);

      Serial.printf("Temperatura: %.1f °C\n", t);
      SerialBT.printf("Temperatura: %.1f °C\n", t);
    }
  }

  // === Lectura LDR ===
  if (leerLuz) {
    leerLuz = false;
    int valorLuz = analogRead(LDRPIN);
    Serial.print("Valor LDR: ");
    Serial.println(valorLuz);

    if (valorLuz > 60) {  // hay mucha luz
      digitalWrite(LED_LUZ, LOW);
    } else {              // poca luz
      digitalWrite(LED_LUZ, HIGH);
    }
  }

  delay(100); // Pequeña pausa general
}
