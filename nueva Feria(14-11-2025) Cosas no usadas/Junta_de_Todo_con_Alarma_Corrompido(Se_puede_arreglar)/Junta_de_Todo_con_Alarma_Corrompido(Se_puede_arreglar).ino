#include "BluetoothSerial.h"
#include "DHT.h"
#include <Ticker.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define FANPIN 5
#define LEDPIN 26
#define LED1 15
#define PIR1 19
#define BUZZER 18

BluetoothSerial SerialBT;
DHT dht(DHTPIN, DHTTYPE);
Ticker ticker;
volatile bool leerTemperatura = false;
String device_name = "ESP32-DHT-BT";

void leerDHT() {
  leerTemperatura = true;
}

void setup() {
  Serial.begin(115200);
  SerialBT.begin(device_name);
  dht.begin();

  pinMode(FANPIN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);

  pinMode(LED1, OUTPUT);
  pinMode(PIR1, INPUT);

  pinMode(BUZZER, OUTPUT);

  Serial.printf("Bluetooth \"%s\" iniciado.\nEmpareja el dispositivo desde tu celular.\n", device_name.c_str());

  ticker.attach(2, leerDHT); // llama a leerDHT cada 2 segundos
}

void loop() {
  value = digitalRead(PIR1);
  // Bluetooth
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
      case 'P':
 	      if (value == 1) {
		      digitalWrite(LED1, HIGH); 
          digitalWrite(BUZZER, HIGH);
          Serial.println("¡intruso!");
          analogWrite(BUZZER, OUTPUT);
          delay(00);
          //digitalWrite(LED,LOW);   		
        }
        else
        {
          digitalWrite(LED1, LOW);
          digitalWrite(BUZZER, LOW);
          Serial.println("seguro");
          delay(00);
        }  
    }
  }

  // Lectura DHT11
  if (leerTemperatura) {
    leerTemperatura = false;

    float t = dht.readTemperature();

    if (isnan(t)) {
      Serial.println("Error al leer el sensor DHT!");
      SerialBT.println("Error al leer el sensor DHT!");
      return;
    }

    // Control del ventilador
    if (t > 28) {
      digitalWrite(FANPIN, HIGH);
    } else {
      digitalWrite(FANPIN, LOW);
    }

    // Enviar datos por Serial y Bluetooth
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println(" °C");

    SerialBT.print("Temperatura: ");
    SerialBT.print(t);
    SerialBT.println(" °C");
  }
 	if (value == 1) {
		digitalWrite(LED1, HIGH); 
    //digitalWrite(BUZZER, HIGH);
    //Serial.println("¡intruso!");
    //analogWrite(BUZZER, OUTPUT);
    delay(00);
    //digitalWrite(LED,LOW);   		
    }
    else
    {
      digitalWrite(LED1, LOW);
      //digitalWrite(BUZZER, LOW);
      //Serial.println("seguro");
      delay(00);
    } 
}