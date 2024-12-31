#include <FastLED.h>

#define LED_PIN     44
#define SENSOR_PIN  7
#define NUM_LEDS    16
#define BRIGHTNESS  250
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR_PIN, INPUT);


  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

}

void loop() {
  // Read the sensor value
  int sensorValue = analogRead(SENSOR_PIN);


  Serial.println(sensorValue);




  
  if (sensorValue < 300) {
  
    fill_solid(leds, NUM_LEDS, CRGB::Blue); 
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }


  FastLED.show();

 

}
