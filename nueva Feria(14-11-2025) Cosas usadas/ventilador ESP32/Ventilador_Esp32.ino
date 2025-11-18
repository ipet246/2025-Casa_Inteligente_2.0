#include "DHT.h" // Incluye la biblioteca DHT
#define DHTPIN 6 // Define el pin digital al que está conectado el DHT11
#define DHTTYPE DHT11 // Especifica que es un sensor DHT11
DHT dht(DHTPIN, DHTTYPE); // Crea una instancia del objeto DHT
int fanpin = 7;

void setup() {
  Serial.begin(9600); // Inicia la comunicación serial
  dht.begin(); // Inicia el sensor
  pinMode(fanpin, OUTPUT);
}

void loop() {
  delay(10); 
  
  float t = dht.readTemperature();
  
  if (isnan(t)) {
    Serial.println(F("Fallo al leer del sensor DHT!"));
    return;
  }
  if (t>40){
    digitalWrite(fanpin, HIGH);
  }
  else{
    digitalWrite(fanpin, LOW); 
  }
  Serial.print(F("%   Temperatura: "));
  Serial.print(t);
  Serial.println(F("°C"));
}