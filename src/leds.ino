#define DATA_PIN    4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    100
#define BRIGHTNESS  32

CRGB leds[NUM_LEDS];
CRGB currentColor = CRGB::Green;

void setup() {
  Serial.begin(9600);
  delay(1000);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  fill_solid(leds, NUM_LEDS, currentColor);
  FastLED.show();

  Serial.println("Commandes disponibles :");
  Serial.println("on / off / on N / off N");
  Serial.println("red / green / blue / white");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    // Couleurs
    if (input.equalsIgnoreCase("red")) {
      currentColor = CRGB::Red;
      fill_solid(leds, NUM_LEDS, currentColor);
      FastLED.show();
      Serial.println("Couleur changée : rouge");
    } else if (input.equalsIgnoreCase("green")) {
      currentColor = CRGB::Green;
      fill_solid(leds, NUM_LEDS, currentColor);
      FastLED.show();
      Serial.println("Couleur changée : vert");
    } else if (input.equalsIgnoreCase("blue")) {
      currentColor = CRGB::Blue;
      fill_solid(leds, NUM_LEDS, currentColor);
      FastLED.show();
      Serial.println("Couleur changée : bleu");
    } else if (input.equalsIgnoreCase("white")) {
      currentColor = CRGB::White;
      fill_solid(leds, NUM_LEDS, currentColor);
      FastLED.show();
      Serial.println("Couleur changée : blanc");
    
    // Commande ON
    } else if (input.startsWith("on")) {
      String param = input.substring(3);
      param.trim();

      if (param.length() == 0) {
        fill_solid(leds, NUM_LEDS, currentColor);
        Serial.println("Toutes les LEDs sont allumées.");
      } else {
        int index = param.toInt();
        if (index >= 0 && index < NUM_LEDS) {
          leds[index] = currentColor;
          Serial.print("LED ");
          Serial.print(index);
          Serial.println(" allumée.");
        } else {
          Serial.println("Numéro de LED invalide.");
        }
      }
      FastLED.show();

    // Commande OFF
    } else if (input.startsWith("off")) {
      String param = input.substring(4);
      param.trim();

      if (param.length() == 0) {
        fill_solid(leds, NUM_LEDS, CRGB::Black);
        Serial.println("Toutes les LEDs sont éteintes.");
      } else {
        int index = param.toInt();
        if (index >= 0 && index < NUM_LEDS) {
          leds[index] = CRGB::Black;
          Serial.print("LED ");
          Serial.print(index);
          Serial.println(" éteinte.");
        } else {
          Serial.println("Numéro de LED invalide.");
        }
      }
      FastLED.show();

    } else {
      Serial.println("Commande inconnue.");
    }
  }
