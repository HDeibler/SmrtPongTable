#include <FastLED.h>

// Constants
#define NUM_LEDS_PER_POD 4
#define DETECTION_THRESHOLD 120

// Pin Definitions
const int sensorPin = 33; // Change to your sensor pin
const int ledPin = 32;    // Change to your LED pin

CRGB leds[NUM_LEDS_PER_POD];

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, ledPin>(leds, NUM_LEDS_PER_POD);
  fill_solid(leds, NUM_LEDS_PER_POD, CRGB::Black);
  pinMode(sensorPin, INPUT);
}

void loop() {
  int sensorValue = analogRead(sensorPin);
  bool detected = sensorValue < DETECTION_THRESHOLD;

  if (detected) {
    fill_solid(leds, NUM_LEDS_PER_POD, CRGB::DarkBlue);
  } else {
    fadeToBlackBy(leds, NUM_LEDS_PER_POD, 40);
  }
   Serial.println(sensorValue);

  FastLED.show();
  delay(10); // Small delay to prevent excessive CPU usage
}
