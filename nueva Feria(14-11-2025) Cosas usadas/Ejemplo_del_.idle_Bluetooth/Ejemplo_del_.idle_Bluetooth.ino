/*
 *  Proyecto: Sombrilla Automática Anti-Lluvia (Versión Simplificada)
 *  Descripción: Mueve UN SOLO servo para abrir o cerrar una sombrilla según la lluvia.
 *  Características clave: Lógica con histeresis para evitar el jitter del servo.
 */

#include <Servo.h>

// --- PINES ---
const int PIN_SERVO = 10;        // Pin de señal del Servo
const int PIN_SENSOR_LLUVIA = A0; // Pin analógico del sensor de lluvia

// --- PARÁMETROS A AJUSTAR ---
// 1. Posiciones de la sombrilla (de 0 a 180)
const int POSICION_DESCUBIERTA = 20;  // Ángulo cuando NO llueve
const int POSICION_PROTEGIDA = 160;   // Ángulo cuando SÍ llueve

// 2. Umbrales de sensibilidad (¡MUY IMPORTANTE!)
// Para calibrar, abre el Monitor Serie (Herramientas -> Monitor Serie)
// y mira los valores cuando el sensor está seco y mojado.
const int UMBRAL_LLUVIA = 400;  // Si el valor es MENOR, se activa la protección.
const int UMBRAL_SECO = 550;    // Si el valor es MAYOR, se retira la protección.

// --- VARIABLES ---
Servo miServo;
int valorSensor = 0;
bool estaProtegido = false; // Estado inicial: la sombrilla está retirada (descubierto)

void setup() {
  // Inicia la comunicación con el ordenador para ver los valores del sensor
  Serial.begin(9600);
  
  // Conecta el servo al pin definido
  miServo.attach(PIN_SERVO);
  
  // Posiciona la sombrilla en su estado inicial (descubierto)
  miServo.write(POSICION_DESCUBIERTA);
  
  Serial.println("Sistema de Sombrilla Automático Iniciado.");
}

void loop() {
  // Lee el valor del sensor de lluvia
  valorSensor = analogRead(PIN_SENSOR_LLUVIA);
  
  // Muestra el valor en el Monitor Serie para que puedas calibrar
  Serial.print("Valor del Sensor: ");
  Serial.println(valorSensor);

  // --- LÓGICA PRINCIPAL ---
  
  // CONDICIÓN 1: Si detecta lluvia Y la sombrilla está descubierta...
  if (valorSensor < UMBRAL_LLUVIA && !estaProtegido) {
    Serial.println(">> Lluvia detectada. Activando protección...");
    miServo.write(POSICION_PROTEGIDA); // Mueve el servo a la posición de protegido
    estaProtegido = true;              // Actualiza el estado
  }
  
  // CONDICIÓN 2: Si el sensor está seco Y la sombrilla está protegida...
  else if (valorSensor > UMBRAL_SECO && estaProtegido) {
    Serial.println(">> Sensor seco. Retirando protección...");
    miServo.write(POSICION_DESCUBIERTA); // Mueve el servo a la posición de descubierto
    estaProtegido = false;               // Actualiza el estado
  }

  // Pequeña pausa para no saturar el sistema
  delay(500); 
}