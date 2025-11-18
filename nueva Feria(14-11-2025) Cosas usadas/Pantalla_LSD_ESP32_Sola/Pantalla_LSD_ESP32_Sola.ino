#include <LiquidCrystal.h>
#include <DHT.h>

#define DHTPIN 7 // Pin de datos del DHT22
#define DHTTYPE DHT22 // Tipo de sensor

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  lcd.begin(16, 2);
  dht.begin();

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
    lcd.setCursor(0, 0);
    lcd.print("Error sensor DHT22");
    lcd.setCursor(0, 1);
    lcd.print("Revisar conexiones");
    delay(2000);
    return;
  }

  // Mostrar temperatura
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t, 1);
  lcd.print((char)223); // símbolo de grados °
  lcd.print("C ");

  // Mostrar humedad
  lcd.setCursor(0, 1);
  lcd.print("Humedad: ");
  lcd.print(h, 1);
  lcd.print("% ");

  delay(2000); // Actualiza cada 2 segundos
}
