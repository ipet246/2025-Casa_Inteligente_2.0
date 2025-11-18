#include "BluetoothSerial.h"
#include "DHT.h"
#include <Ticker.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define FANPIN 5
#define LEDPIN 26       // LED controlado por Bluetooth
#define LDRPIN 34       // Entrada analógica del LDR (pin ADC)
#define LED_LUZ 27      // LED controlado por el LDR

BluetoothSerial SerialBT;
DHT dht(DHTPIN, DHTTYPE);

// Timers
Ticker tickerDHT;
Ticker tickerLuz;

// Variables
volatile bool leerTemperatura = false;
volatile bool leerLuz = false;

String device_name = "ESP32-DHT-LUZ-BT";

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

  Serial.printf("Bluetooth \"%s\" iniciado.\nEmpareja el dispositivo desde tu celular.\n", device_name.c_str());

  // Timers
  tickerDHT.attach(2, activarLecturaDHT);  // cada 2 s lee DHT
  tickerLuz.attach(1, activarLecturaLuz);  // cada 1 s lee LDR
}

void loop() {
  // Bluetooth
  if (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.write(c);

    switch (c) {
      case 'A': // Encender LED manualmente
        digitalWrite(LEDPIN, HIGH);
        SerialBT.println("LED encendido manualmente.");
        break;

      case 'B': // Parpadear LED
        digitalWrite(LEDPIN, LOW);
        delay(200);
        digitalWrite(LEDPIN, HIGH);
        SerialBT.println("LED parpadeó.");
        break;

      case 'C': // Apagar LED manualmente
        digitalWrite(LEDPIN, LOW);
        SerialBT.println("LED apagado manualmente.");
        break;
    }
  }

  // Lectura DHT 
  if (leerTemperatura) {
    leerTemperatura = false;
    float t = dht.readTemperature();

    if (isnan(t)) {
      Serial.println("Error al leer el sensor DHT!");
      SerialBT.println("Error al leer el sensor DHT!");
    } else {
      // Control ventilador
      if (t > 25)
        digitalWrite(FANPIN, HIGH);
      else
        digitalWrite(FANPIN, LOW);

      Serial.printf("Temperatura: %.1f °C\n", t);
      SerialBT.printf("Temperatura: %.1f °C\n", t);
    }
  }

  //  Lectura LDR
  if (leerLuz) {
    leerLuz = false;
    int valorLuz = analogRead(LDRPIN);

    Serial.print("Valor LDR: ");
    Serial.println(valorLuz);
    SerialBT.printf("Luz: %d\n", valorLuz);

    // Ajustá el umbral según la iluminación de tu ambiente
    if (valorLuz > 200) {  // mucha luz → apaga LED
      digitalWrite(LED_LUZ, LOW);
    } else {                // poca luz → enciende LED
      digitalWrite(LED_LUZ, HIGH);
    }
  }
}

