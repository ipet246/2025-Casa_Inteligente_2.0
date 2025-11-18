// Incluir librerias
#include <LiquidCrystal.h>
#include <DHT.h>

// Configuración de pines
#define DHTPIN 13         // Pin de datos del DHT22
#define DHTTYPE DHT22     // Tipo de sensor

// Pines del LCD (usa pines compatibles con ESP32)
LiquidCrystal lcd(19, 18, 5, 4, 2, 15);

// Objetos
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Iniciando...");
  delay(1500);
  lcd.clear();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Si hay error de lectura
  if (isnan(h) || isnan(t)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error sensor DHT22");
    lcd.setCursor(0, 1);
    lcd.print("Revisar conexion");
    Serial.println("Error al leer el DHT22");
    delay(2000);
    return;
  }

  // Mostrar temperatura
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t, 1);
  lcd.print((char)223); // símbolo de grados °
  lcd.print("C   ");

  // Mostrar humedad
  lcd.setCursor(0, 1);
  lcd.print("Humedad: ");
  lcd.print(h, 1);
  lcd.print("%   ");

  delay(2000); // Actualiza cada 2 segundos
}
