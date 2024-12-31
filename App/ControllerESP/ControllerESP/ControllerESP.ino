/*Created by Hunter Deibler*/
#include "BeerPong.h"
#include "Table.h"
#include "BeerBall.h"
#include "Config.h"
#include "SledBob.h"


#define DEBOUNCE_MS 200

// Game Modes
Table pongTable;
BeerPong beerPongMode;
BeerBall beerBallMode;
SledBob sledBobMode;

// State Variables
bool pongMode = true;
bool beerBall = false;
bool sledBob = false;
bool startup = false;

HardwareSerial MySerial1(1);
HardwareSerial MySerial2(2);

CRGB leds1[NUM_LEDS_PER_STRIP];
CRGB leds2[NUM_LEDS_PER_STRIP];
CRGB leds3[NUM_LEDS_PER_STRIP];
CRGB leds4[NUM_LEDS_PER_STRIP];
CRGB * ledsArray[] = {
  leds1,
  leds2,
  leds3,
  leds4
};

unsigned long lastButtonPressTime = 0;

// Current Mode
String currentMode = "BeerPong";

void setup() {
  Serial.begin(9600);
  MySerial1.begin(31250, SERIAL_8N1, RXp1, TXp1);
  MySerial2.begin(19200, SERIAL_8N1, RXp2, TXp2);
  delay(3000);
  // Initialize LED pins as outputs
  pinMode(BB1_LED_PIN, OUTPUT);
  pinMode(BB2_LED_PIN, OUTPUT);
  pinMode(BB3_LED_PIN, OUTPUT);
  pinMode(BB4_LED_PIN, OUTPUT);

  // Initialize sensor pins as inputs
  pinMode(BB1_SENSOR_PIN, INPUT);
  pinMode(BB2_SENSOR_PIN, INPUT);
  pinMode(BB3_SENSOR_PIN, INPUT);
  pinMode(BB4_SENSOR_PIN, INPUT);

  FastLED.addLeds < LED_TYPE, BB1_LED_PIN, COLOR_ORDER > (leds1, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds < LED_TYPE, BB2_LED_PIN, COLOR_ORDER > (leds2, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds < LED_TYPE, BB3_LED_PIN, COLOR_ORDER > (leds3, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.addLeds < LED_TYPE, BB4_LED_PIN, COLOR_ORDER > (leds4, NUM_LEDS_PER_STRIP).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  pinMode(GAME_MODE_PIN_1, INPUT_PULLUP);
  pinMode(GAME_MODE_PIN_2, INPUT_PULLUP);
  pinMode(GAME_MODE_PIN_3, INPUT_PULLUP);

}

void loop() {
  if (!startup) {
    setupGameMode();
    startup = true;
  }

  // Check for button presses to change game modes
  if (millis() - lastButtonPressTime > DEBOUNCE_MS) {
    if (digitalRead(GAME_MODE_PIN_1) == LOW) {
      lastButtonPressTime = millis();
      if (currentMode != "BeerPong") {
        switchGameMode("BeerPong");
      }
    } else if (digitalRead(GAME_MODE_PIN_2) == LOW) {
      lastButtonPressTime = millis();
      if (currentMode != "BeerBall") {
        switchGameMode("BeerBall");
      }
    } else if (digitalRead(GAME_MODE_PIN_3) == LOW) {
      lastButtonPressTime = millis();
      if (currentMode != "SledBob") {
        switchGameMode("SledBob");
      }
    }
  }

  if (pongMode && startup) {
    bool stateLeftUpdated = false;
    bool stateRightUpdated = false;
    if (MySerial1.available()) {
      String dataLeft = MySerial1.readStringUntil('\n');
      dataLeft = dataLeft.substring(1); // Remove the first character 'L'
      pongTable.updateCurrentGame(pongTable, dataLeft, 'L');
      stateLeftUpdated = true;
    }

    if (MySerial2.available()) {
      String dataRight = MySerial2.readStringUntil('\n');
      dataRight = dataRight.substring(1); // Remove the first character 'R'
      pongTable.updateCurrentGame(pongTable, dataRight, 'R');
      stateRightUpdated = true;

    }

    if (stateLeftUpdated || stateRightUpdated) {
      String ledCommandsCombined = pongTable.generateLEDCommands();
      int delimiterPos = ledCommandsCombined.indexOf('|');
      String leftCommands = ledCommandsCombined.substring(0, delimiterPos);
      String rightCommands = ledCommandsCombined.substring(delimiterPos + 1);

      MySerial1.println(leftCommands);
      MySerial2.println(rightCommands);
    }
  } else if (beerBall) {
    bool stateLeftUpdated = false;
    bool stateRightUpdated = false;
    if (MySerial1.available()) {
      String dataLeft = MySerial1.readStringUntil('\n');
      dataLeft = dataLeft.substring(1); // Remove the first character 'L'
      pongTable.updateCurrentGame(pongTable, dataLeft, 'L');
      stateLeftUpdated = true;
    }

    if (MySerial2.available()) {
      String dataRight = MySerial2.readStringUntil('\n');
      dataRight = dataRight.substring(1); // Remove the first character 'R'
      pongTable.updateCurrentGame(pongTable, dataRight, 'R');
      stateRightUpdated = true;

    }

    if (stateLeftUpdated || stateRightUpdated) {
      String ledCommandsCombined = pongTable.generateLEDCommands();
      int delimiterPos = ledCommandsCombined.indexOf('|');
      String leftCommands = ledCommandsCombined.substring(0, delimiterPos);
      String rightCommands = ledCommandsCombined.substring(delimiterPos + 1);

      MySerial1.println(leftCommands);
      MySerial2.println(rightCommands);
    }
  } else {
    try {
      bool stateLeftUpdated = false;
      bool stateRightUpdated = false;
      if (MySerial1.available()) {
        String dataLeft = MySerial1.readStringUntil('\n');
        dataLeft = dataLeft.substring(1);
        pongTable.updateCurrentGame(pongTable, dataLeft, 'L');
        stateLeftUpdated = true;
      }
      if (MySerial2.available()) {
        String dataRight = MySerial2.readStringUntil('\n');
        dataRight = dataRight.substring(1);
        pongTable.updateCurrentGame(pongTable, dataRight, 'R');
        stateRightUpdated = true;

      }
      if (stateLeftUpdated || stateRightUpdated) {
        String ledCommandsCombined = pongTable.generateLEDCommands();
        int delimiterPos = ledCommandsCombined.indexOf('|');
        String leftCommands = ledCommandsCombined.substring(0, delimiterPos);
        String rightCommands = ledCommandsCombined.substring(delimiterPos + 1);
        MySerial1.println(leftCommands);
        MySerial2.println(rightCommands);
      }
    } catch (const std::exception & e) {
      Serial.print("Exception caught in updateGameLogic: ");
      Serial.println(e.what());
    } catch (...) {
      Serial.println("Unknown exception caught in updateGameLogic");
    }

  }
}

void setupGameMode() {
  MySerial1.println("startup");
  MySerial2.println("startup");
  pongTable.setCurrentGameMode( & beerPongMode);
  currentMode = "BeerPong"; // Set the initial game mode
}

void switchGameMode(String mode) {

  if (mode == "BeerPong") {
    sledBob = false;
    beerBall = false;
    MySerial1.println("startup");
    MySerial2.println("startup");
    pongTable.clearLeds();
    pongTable.setCurrentGameMode( & beerPongMode);
    pongMode = true;
  } 
  
  else if (mode == "BeerBall") {
    pongMode = false;
    sledBob = false;
    MySerial1.println("-1");
    MySerial2.println("-1");
    pongTable.clearLeds();
    pongTable.setCurrentGameMode( & beerBallMode);
    MySerial1.println("beerball");
    MySerial2.println("beerball");
    pongTable.bootUp();
    beerBall = true;
  } 
  
  else if (mode == "SledBob") {
    pongMode = false;
    beerBall = false;
    MySerial1.println("startup");
    MySerial2.println("startup");
    pongTable.setCurrentGameMode( & sledBobMode);
    sledBob = true;
  }

  currentMode = mode;

}