#include <Wire.h>
#include "Adafruit_SHTC3.h"

Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

// PIR
const int PIR_POWER = A2;
const int PIR_SIGNAL = A3;

int lastPirState = LOW;
unsigned long lastTempRead = 0;
const unsigned long TEMP_INTERVAL = 10000; // 10 secondes

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Initialisation capteur PIR
  pinMode(PIR_POWER, OUTPUT);
  digitalWrite(PIR_POWER, HIGH);
  pinMode(PIR_SIGNAL, INPUT);
  Serial.println("Capteur PIR prêt.");

  // Initialisation capteur SHTC3
  if (!shtc3.begin()) {
    Serial.println("Erreur : SHTC3 non détecté.");
    while (1) delay(1);
  }
  Serial.println("Capteur SHTC3 détecté.");
}

void loop() {
  // Vérifie mouvement PIR
  int pirState = digitalRead(PIR_SIGNAL);
  if (pirState == HIGH && lastPirState == LOW) {
    Serial.println("🔔 Mouvement détecté !");
  }
  lastPirState = pirState;

  // Lecture température/humidité
  if (millis() - lastTempRead >= TEMP_INTERVAL) {
    sensors_event_t humidity, temp;
    if (shtc3.getEvent(&humidity, &temp)) {
      Serial.print("🌡 Température : ");
      Serial.print(temp.temperature);
      Serial.print(" °C  |  💧 Humidité : ");
      Serial.print(humidity.relative_humidity);
      Serial.println(" %");
    } else {
      Serial.println("Erreur de lecture du SHTC3 !");
    }
    lastTempRead = millis();
  }

  delay(100);
}
