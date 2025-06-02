#include <FastLED.h>
#include <Wire.h>
#include "Adafruit_SHTC3.h"

#define DATA_PIN    4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    100
#define BRIGHTNESS  32

CRGB leds[NUM_LEDS];
Adafruit_SHTC3 shtc3 = Adafruit_SHTC3();

// PIR
const int PIR_POWER = A2;
const int PIR_SIGNAL = A3;
int pirState = LOW;
int lastPirState = LOW;

// Timer pour SHTC3
unsigned long lastTempRead = 0;
const unsigned long TEMP_INTERVAL = 30; // 30 secondes

void setup() {
  Serial.begin(9600);
  delay(3000);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  fill_solid(leds, NUM_LEDS, CRGB::Green);
  FastLED.show();

  Serial.println("Commandes : 'on [couleur] [N]', 'off [N]'");

  pinMode(PIR_POWER, OUTPUT);
  pinMode(PIR_SIGNAL, INPUT);
  digitalWrite(PIR_POWER, HIGH);
  Serial.println("Capteur PIR initialisé");

  if (!shtc3.begin()) {
    Serial.println("Capteur SHTC3 non détecté !");
    while (1) delay(1);
  }
  Serial.println("SHTC3 détecté !");
}

void loop() {
  // PIR
  pirState = digitalRead(PIR_SIGNAL);
  if (pirState != lastPirState) {
    if (pirState == HIGH) {
      Serial.println("Un mouvement détecté");
    }
    lastPirState = pirState;
  }

  // Commandes LED
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    // Commande "off"
    if (input.startsWith("off")) {
      String param = input.substring(4);
      param.trim();

      if (param.length() == 0) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        Serial.println("Toutes les LEDs sont éteintes.");
      } else {
        int index = param.toInt();
        if (index >= 0 && index < NUM_LEDS) {
          leds[index] = CRGB::Black;
          FastLED.show();
          Serial.print("LED ");
          Serial.print(index);
          Serial.println(" éteinte.");
        } else {
          Serial.print("Numéro invalide. (0 à ");
          Serial.print(NUM_LEDS - 1);
          Serial.println(")");
        }
      }

    // Commande "on"
    } else if (input.startsWith("on")) {
      String param = input.substring(3);
      param.trim();

      String colorStr = "";
      String indexStr = "";
      int spaceIndex = param.indexOf(' ');

      if (spaceIndex == -1) {
        // un seul paramètre : soit couleur soit index
        if (param.length() > 0 && isDigit(param.charAt(0))) {
          indexStr = param;
        } else {
          colorStr = param;
        }
      } else {
        colorStr = param.substring(0, spaceIndex);
        indexStr = param.substring(spaceIndex + 1);
        indexStr.trim();
      }

      // couleur par défaut : green
      CRGB color = CRGB::Green;
      if (colorStr == "red") color = CRGB::Red;
      else if (colorStr == "blue") color = CRGB::Blue;
      else if (colorStr == "white") color = CRGB::White;
      else if (colorStr == "green") color = CRGB::Green;
      else if (colorStr != "" && !isDigit(colorStr.charAt(0))) {
        Serial.println("Couleur inconnue. Exemples : red, green, blue, white");
        return;
      }

      if (indexStr == "") {
        fill_solid(leds, NUM_LEDS, color);
        FastLED.show();
        Serial.print("Toutes les LEDs allumées en ");
        Serial.println(colorStr == "" ? "vert" : colorStr);
      } else {
        int index = indexStr.toInt();
        if (index >= 0 && index < NUM_LEDS) {
          leds[index] = color;
          FastLED.show();
          Serial.print("LED ");
          Serial.print(index);
          Serial.print(" allumée en ");
          Serial.println(colorStr == "" ? "vert" : colorStr);
        } else {
          Serial.print("Numéro invalide. (0 à ");
          Serial.print(NUM_LEDS - 1);
          Serial.println(")");
        }
      }

    } else {
      Serial.println("Commande inconnue. Exemples : on red, on blue 5, off, off 10");
    }
  }

  // Température/Humidité
  if (millis() - lastTempRead >= TEMP_INTERVAL) {
    sensors_event_t humidity, temp;
    if (!shtc3.getEvent(&humidity, &temp)) {
      Serial.println("Erreur de lecture du SHTC3 !");
    } else {
      Serial.print("{\"temperature\": ");
      Serial.print(temp.temperature);
      Serial.print(", \"humidite\": ");
      Serial.print(humidity.relative_humidity);
      Serial.println(" }");
    }
    lastTempRead = millis();
  }

  delay(100);
}


