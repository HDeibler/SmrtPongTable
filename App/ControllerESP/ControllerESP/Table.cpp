#include "Table.h"
#include "Config.h"
#include "Cup.h"
#include "BeerBall.h"
#include "BeerPong.h"
#include "SledBob.h"


Table::Table() : currentGameMode(nullptr), leftSet("Left"), rightSet("Right"), matrix() {
  matrix.setupMatrix();

  rightSet.initializeSideCups(1, 2, 36, 3);
  leftSet.initializeSideCups(4, 5, 7, 15);
  
  leftSet.getSideCups(0).setLedStripIndex(2);
  leftSet.getSideCups(1).setLedStripIndex(3);
  rightSet.getSideCups(0).setLedStripIndex(0);
  rightSet.getSideCups(1).setLedStripIndex(1);
}

void Table::setCurrentGameMode(GameMode* mode) {
    if (mode->getType() == GameModeType::BeerBall) {
        matrix.clearDisplay();
        matrix.beerBallStartUp();
        delay(2000);
        matrix.clearDisplay();
        matrix.updateBeerBallDisplay();
        
    } else if(mode->getType() == GameModeType::BeerPong) {
        this->clearLeds();
        matrix.pongStartUp();
    }
      else if(mode->getType() == GameModeType::SledBob){
        matrix.clearDisplay();
        this->clearLeds();
        this->bootUp();

        this->getCupSet("Left").getSideCups(0).setActivated(false);
        this->getCupSet("Left").getSideCups(1).setActivated(false);
        
        this->getCupSet("Right").getSideCups(0).setActivated(false);
        this->getCupSet("Right").getSideCups(1).setActivated(false);

        this->getCupSet("Right").setBothSideCupsActivated(false);
        this->getCupSet("Left").setBothSideCupsActivated(false);   
    }
    currentGameMode = mode;
  }





void Table::updateCurrentGame(Table& table, String& data, char side) {
    if (currentGameMode) {
        currentGameMode->handleData(table, data, side);
    }
}



String Table::generateLEDCommands() {
    if (currentGameMode) {
        return currentGameMode->createLEDCommands(*this); 
    }
    return ""; 
}

String Table::getCurrentGameMode(Table& table) {
    if (currentGameMode) {
        String ledCommands = currentGameMode->createLEDCommands(table);
    }
}

void Table::updateCupSet(const int sensorValues[], char side) {
    
    if (side == 'L') {
        leftSet.updateState(sensorValues, side);
    } else if (side == 'R') {
        rightSet.updateState(sensorValues, side);
    }
    if(currentGameMode->getType() == GameModeType::BeerPong ) {
      updateDisplay();
    }

}

CupStatus Table::checkCupStatus(int index, const String& side) {
    if (getCup(index, side).isBermudaCup()) {
        return Bermuda;
    } else if (getCup(index, side).isIslandCup()) {
        return Island;
    } else if (getCup(index, side).isWestVirginiaCup()){
        return WestVirginia;
    } else if (getCup(index, side).isLastCup()){
      return LastCup;
    } else if (getCup(index, side).isRedLightCup()){
      return RedLight;
    }


    return Regular;
}

Cup& Table::getCup(int index, const String& side) {
    if (side == "Left") {
        return leftSet.getCup(index);
    } else if (side == "Right") {
        return rightSet.getCup(index);
    }

}

void Table::printLeftCupStates() {
    Serial.println("-----------------------------");
    Serial.println("Left Cup Set States:");
    for (int i = 0; i < leftSet.getNumCups(); i++) {
        Serial.print(leftSet.getCup(i).getName() + ": ");
        Serial.println(leftSet.getCup(i).isCupPresent() ? "Present" : "Absent");
    }
    Serial.println();
}

void Table::printRightCupStates() {
    Serial.println("-----------------------------");
    Serial.println("Right Cup Set States:");
    for (int i = 0; i < rightSet.getNumCups(); i++) {
        Serial.print(rightSet.getCup(i).getName() + ": ");
        Serial.println(rightSet.getCup(i).isCupPresent() ? "Present" : "Absent");
    }
    Serial.println();
}

CupSet& Table::getCupSet(const String& side) {
    if (side == "Left") {
        return leftSet;
    } else if (side == "Right") {
        return rightSet;
    }

}

void Table::updateMiddleSensorState(int sensorValue, char side) {
if (side == 'L') {
        leftSet.updateMiddleSensorState(sensorValue);
    } else if (side == 'R') {
        rightSet.updateMiddleSensorState(sensorValue);
    }
}

bool Table::getMiddleSensorState(const String& side) {
    if (side == "Left") {
        return leftSet.getMiddleSensorState();
    } else if (side == "Right") {
        return rightSet.getMiddleSensorState();
    }
    return false; // Default case
}

void Table::updateDisplay() {
  matrix.updateDisplay(leftSet, rightSet);
}

void Table::clearLeds(){
  for(int i = 0; i < 4; i++){
    fill_solid(ledsArray[i], NUM_LEDS_PER_STRIP, CRGB::Black); 
  }
}

void Table::startUp() {
    const int cycles = 3; 
    const int transitionSteps = 30; 
    const int stepDuration = 20; 


    CHSV colors[] = {CHSV(HUE_BLUE, 255, 255), CHSV(HUE_RED, 255, 255), CHSV(HUE_GREEN, 255, 255), CHSV(HUE_YELLOW, 255, 255)};
    const int numColors = sizeof(colors) / sizeof(colors[0]);
    // Start the animation cycles
    for (int cycle = 0; cycle < cycles; cycle++) {
        for (int colorIndex = 0; colorIndex < numColors; colorIndex++) {
            // Gradually transition to the next color in the sequence
            for (int step = 0; step < transitionSteps; step++) {
                for (int podIndex = 0; podIndex < 4; podIndex++) {
                    int nextColorIndex = (colorIndex + podIndex) % numColors;
                    CHSV currentColor = blend(colors[(nextColorIndex + numColors - 1) % numColors], colors[nextColorIndex], step * 255 / transitionSteps);
                    fill_solid(ledsArray[podIndex], NUM_LEDS_PER_STRIP, currentColor);
                }
                FastLED.show();
                delay(stepDuration);
            }
        }
    }
    // Finish with a final display of the last color to all pods
    for (int i = 0; i < 4; i++) {
        fill_solid(ledsArray[i], NUM_LEDS_PER_STRIP, colors[(numColors - 1)]);
    }
    FastLED.show();
    delay(500);
    for (int i = 0; i < 4; i++) {
        fill_solid(ledsArray[i], NUM_LEDS_PER_STRIP, CRGB::Black);
    }
    FastLED.show();
    delay(500);

    // Re-light all pods for game start
    for (int i = 0; i < 4; i++) {
        fill_solid(ledsArray[i], NUM_LEDS_PER_STRIP, colors[0]);
    }
    FastLED.show();
    delay(100);
    
}



void Table::bootUp() {
    const unsigned long pulseDuration = 5000; 
    const unsigned long startTime = millis();
    const float twoPi = 6.28318530718; // 2 * PI
    const int pulseCycleDuration = 1000; 

    while (millis() - startTime < pulseDuration) {
        unsigned long elapsed = millis() - startTime;
        float phase = (float)(elapsed % pulseCycleDuration) / pulseCycleDuration;
        float sineValue = (sin(phase * twoPi) + 1.0) / 2.0; // Sine wave for smooth pulsing
        uint8_t brightness = 255 * sineValue; // Scale brightness from 0 to 255

        CRGB pulsingBlue = CHSV(HUE_BLUE, 255, brightness);

        for (int podIndex = 0; podIndex < 4; ++podIndex) {
            fill_solid(ledsArray[podIndex], 16, pulsingBlue);
        }
        
        FastLED.show();
        delay(10); // Short delay for smooth transition
    }

    // After the animation, reset colors or turn off LEDs as needed
    for (int podIndex = 0; podIndex < 4; ++podIndex) {
        fill_solid(ledsArray[podIndex], 16, CRGB::Black);
    }
    FastLED.show();
}









