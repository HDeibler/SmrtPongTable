#include <FastLED.h>
#include <HardwareSerial.h>
#include <FastLED_NeoMatrix.h>
#include <Adafruit_GFX.h>
#pragma once

// RX/TX Pins
#define RXp1 18
#define TXp1 17
#define RXp2 37
#define TXp2 38

// Game Mode Pins
#define GAME_MODE_PIN_1 9
#define GAME_MODE_PIN_2 10
#define GAME_MODE_PIN_3 11

// Pin definitions for BeerBall mode
#define BB1_LED_PIN    1
#define BB1_SENSOR_PIN 2
#define BB2_LED_PIN    36
#define BB2_SENSOR_PIN 3
#define BB3_LED_PIN    4
#define BB3_SENSOR_PIN 5
#define BB4_LED_PIN    7
#define BB4_SENSOR_PIN 15


//BBall pod definitions
#define NUM_LEDS_PER_STRIP 16
#define BRIGHTNESS  80
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
extern CRGB leds1[NUM_LEDS_PER_STRIP];
extern CRGB leds2[NUM_LEDS_PER_STRIP];
extern CRGB leds3[NUM_LEDS_PER_STRIP];
extern CRGB leds4[NUM_LEDS_PER_STRIP];
extern CRGB* ledsArray[];


//Pin definition and Led Matrix Setup
#define PIN 40
#define MATRIX_WIDTH 22
#define MATRIX_HEIGHT 20
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)


