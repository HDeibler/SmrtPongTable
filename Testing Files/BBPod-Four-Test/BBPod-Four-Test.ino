#include <FastLED.h>

// Define pins for LED strips and sensors
#define BB1_LED_PIN    1
#define BB1_SENSOR_PIN 2

#define BB2_LED_PIN    36
#define BB2_SENSOR_PIN 3

#define BB3_LED_PIN    4
#define BB3_SENSOR_PIN 5

#define BB4_LED_PIN    7
#define BB4_SENSOR_PIN 15


#define NUM_LEDS_PER_STRIP 16
#define BRIGHTNESS  40
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define SENSOR_THRESHOLD 300 // Adjust this threshold as needed

CRGB leds1[NUM_LEDS_PER_STRIP];
CRGB leds2[NUM_LEDS_PER_STRIP];
CRGB leds3[NUM_LEDS_PER_STRIP];
CRGB leds4[NUM_LEDS_PER_STRIP];

void setup() {
  Serial.begin(115200);
  
  FastLED.addLeds<LED_TYPE, BB1_LED_PIN, COLOR_ORDER>(leds1, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, BB2_LED_PIN, COLOR_ORDER>(leds2, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, BB3_LED_PIN, COLOR_ORDER>(leds3, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE, BB4_LED_PIN, COLOR_ORDER>(leds4, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
  // Read the sensor values
  int BB1_value = analogRead(BB1_SENSOR_PIN);
  int BB2_value = analogRead(BB2_SENSOR_PIN);
  int BB3_value = analogRead(BB3_SENSOR_PIN);
  int BB4_value = analogRead(BB4_SENSOR_PIN);

  Serial.print("Pod 1: ");
  Serial.println(BB1_value);

  Serial.print("Pod 2: ");
  Serial.println(BB2_value);

  Serial.print("Pod 3: ");
  Serial.println(BB3_value);

  Serial.print("Pod 4: ");
  Serial.println(BB4_value);

  // Set LEDs for BB1
  if (BB1_value < SENSOR_THRESHOLD) {
    fill_solid(leds1, NUM_LEDS_PER_STRIP, CRGB::Blue);
  } else {
    fill_solid(leds1, NUM_LEDS_PER_STRIP, CRGB::Black);
  }

  // Set LEDs for BB2
  if (BB2_value < SENSOR_THRESHOLD) {
    fill_solid(leds2, NUM_LEDS_PER_STRIP, CRGB::Blue);
  } else {
    fill_solid(leds2, NUM_LEDS_PER_STRIP, CRGB::Black);
  }

  // Set LEDs for BB3
  if (BB3_value < SENSOR_THRESHOLD) {
    fill_solid(leds3, NUM_LEDS_PER_STRIP, CRGB::Blue);
  } else {
    fill_solid(leds3, NUM_LEDS_PER_STRIP, CRGB::Black);
  }

    if (BB4_value < SENSOR_THRESHOLD) {
    fill_solid(leds4, NUM_LEDS_PER_STRIP, CRGB::Blue);
  } else {
    fill_solid(leds4, NUM_LEDS_PER_STRIP, CRGB::Black);
  }

  // Update the LED strips
  FastLED.show();
}
