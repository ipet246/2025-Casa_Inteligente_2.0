#include <LiquidCrystal.h>
#include <DHT.h>
#include "BluetoothSerial.h"
#include <Ticker.h>

// ==== Pines ====
// --- Sensores ---
#define DHTPIN 13          // Pin de datos del DHT22
#define DHTTYPE DHT22      // Tipo de sensor
#define LDRPIN 33          // Sensor LDR

// --- Actuadores ---
#define FANPIN 23          // Ventilador (usa transistor o relay)
#define LED_LUZ 27         // LED controlado por luz
#define LED_BT 26          // LED controlado por Bluetooth

// --- LCD (modo paralelo) ---
LiquidCrystal lcd(19, 18, 5, 4, 2, 15);

// ==== Objetos ====
BluetoothSerial SerialBT;
DHT dht(DHTPIN, DHTTYPE);
Ticker tickerDHT;
Ticker tickerLuz;

volatile bool leerTemperatura = false;
volatile bool leerLuz = false;
String device_name = "ESP32-COMPLETO";

// ==== Variables ====
bool modoAlarma = false;
float temperatura = 0;
float humedad = 0;
int valorLuz = 0;

// ==== Funciones de los timers ====
void activarLecturaDHT() { leerTemperatura = true; }
void activarLecturaLuz() { leerLuz = true; }

// ==== Setup ====
void setup() {
  Serial.begin(115200);
  SerialBT.begin(device_name);
  dht.begin();

  pinMode(FANPIN, OUTPUT);
  pinMode(LED_BT, OUTPUT);
  pinMode(LED_LUZ, OUTPUT);
  pinMode(LDRPIN, INPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Iniciando...");
  delay(1500);
  lcd.clear();

  Serial.printf("Bluetooth \"%s\" iniciado.\nEmpareja el dispositivo desde tu celular.\n", device_name.c_str());
  Serial.println("Sistema listo (DHT22 + LDR + LCD + BT)");

  // Timers
  tickerDHT.attach(2, activarLecturaDHT);
  tickerLuz.attach(1, activarLecturaLuz);
}

// ==== Loop ====
void loop() {
  // === Bluetooth ===
  if (SerialBT.available()) {
    char c = SerialBT.read();
    Serial.write(c);

    switch (c) {
      case 'A': // Encender LED por Bluetooth
        digitalWrite(LED_BT, HIGH);
        SerialBT.println("LED Bluetooth encendido.");
        break;

      case 'B': // Parpadear LED por Bluetooth
        digitalWrite(LED_BT, LOW);
        delay(200);
        digitalWrite(LED_BT, HIGH);
        SerialBT.println("LED parpadeó.");
        break;

      case 'C': // Activar/desactivar modo alarma
        modoAlarma = !modoAlarma;
        SerialBT.printf("Modo alarma: %s\n", modoAlarma ? "Activado" : "Desactivado");
        break;
    }
  }

  // === Lectura DHT22 ===
  if (leerTemperatura) {
    leerTemperatura = false;
    temperatura = dht.readTemperature();
    humedad = dht.readHumidity();

    if (isnan(temperatura) || isnan(humedad)) {
      Serial.println("Error al leer el sensor DHT22!");
      SerialBT.println("Error al leer el sensor DHT22!");
      lcd.clear();
      lcd.print("Error DHT22");
    } 
    else {
      if (temperatura > 38){
        digitalWrite(FANPIN, HIGH);
      }
      else{
        digitalWrite(FANPIN, LOW);
      }
      Serial.printf("Temp: %.1f°C  Hum: %.1f%%\n", temperatura, humedad);
      SerialBT.printf("Temp: %.1f°C  Hum: %.1f%%\n", temperatura, humedad);

      // Mostrar en LCD
      lcd.setCursor(0, 0);
      lcd.print("T:");
      lcd.print(temperatura, 1);
      lcd.print((char)223);
      lcd.print("C ");

      lcd.print("H:");
      lcd.print(humedad, 0);
      lcd.print("% ");
    }
  }

  // === Lectura LDR ===
  if (leerLuz) {
    leerLuz = false;
    valorLuz = analogRead(LDRPIN);
    Serial.print("Valor LDR: ");
    Serial.println(valorLuz);

    if (valorLuz > 60) {  // hay mucha luz
      digitalWrite(LED_LUZ, LOW);
    } else {              // poca luz
      digitalWrite(LED_LUZ, HIGH);
    }

    // Mostrar luz en LCD
    lcd.setCursor(0, 1);
    lcd.print("Luz: ");
    lcd.print(valorLuz);
    lcd.print("    "); // limpia restos
  }

  delay(100);
}
