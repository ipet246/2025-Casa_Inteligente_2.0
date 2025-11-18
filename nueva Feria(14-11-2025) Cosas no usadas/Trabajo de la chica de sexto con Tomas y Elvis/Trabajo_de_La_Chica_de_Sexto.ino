// C++ code

int humedad = 0;//guarda el valor de la humedad como numerioc

int ledVerde = 5;    // Suelo normal
int ledAmarillo = 2; // Suelo seco
int ledRojo = 4;     // Suelo moojado


void setup()
{  
  Serial.begin(9600);//inicia la comunicacion para mostrar los datos en el monitor
  pinMode(ledVerde, OUTPUT);
  pinMode(ledAmarillo, OUTPUT);
  pinMode(ledRojo, OUTPUT);
  
}

void loop()
{
  humedad = analogRead(A0);
  Serial.print("Humedad = ");//muestrta el valor de humedad en el monitor
  Serial.println(humedad);

  if (humedad < 200) {// SECO = activar riego
    digitalWrite(ledAmarillo, HIGH);
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledRojo, LOW);
    Serial.println("💧 suelo sec");
  }
  if {// MOJADO → sin riego
    digitalWrite(ledRojo, HIGH);
    digitalWrite(ledVerde, LOW);
    digitalWrite(ledAmarillo, LOW);
    Serial.println("💦 Suelo mojado");
  }}
   else if (humedad < 600) {     // NORMAL → sin riego
    digitalWrite(ledVerde, HIGH);   
    digitalWrite(ledAmarillo, LOW);
    digitalWrite(ledRojo, LOW);
    Serial.println("🌿 Suelo normal");
  
  }

  delay(1000);
}