#include "CupSet.h"
#include "Config.h"

CupSet::CupSet(String sideLabel) 
: side(sideLabel), middleSensorActive(false), debounceCounter(0),
  cups{Cup("Front Cup"), Cup("Middle Right Cup"), Cup("Middle Left Cup"), Cup("Back Right Cup"), Cup("Back Middle Cup"), Cup("Back Left Cup")}, SideCups{Cup("One"), Cup("Two")} {
  currentState = BeerPongState::CS_Regular; 
}

Cup& CupSet::getCup(int index) {
    return cups[index];
}

void CupSet::setSpecialState(BeerPongState state) {
    currentState = state;
}

String CupSet::getSide() {
    return side;
    Serial.print(side);
}

int CupSet::getNumCups() const {
    return NUM_CUPS;
}

void CupSet::updateState(const int sensorValues[], char side) {
    for (int i = 0; i < NUM_CUPS; i++) {
        
        cups[i].updateState(sensorValues[i], side, i);
    }
}

int CupSet::getNumCupsConstant() {
    return NUM_CUPS;
}
void CupSet::updateMiddleSensorState(int sensorValue) {
    const int threshold = 18; 
    const int debounceThreshold = 5; // Number of consistent readings required for state change

    if (sensorValue < threshold) {
        if (debounceCounter < debounceThreshold) {
            debounceCounter++; // Increment debounce counter if reading is below threshold
        }
        if (debounceCounter >= debounceThreshold) {
            // Change state only if consistent readings have been met
            middleSensorActive = true;
        }
    } else {
        if (debounceCounter > -debounceThreshold) {
            debounceCounter--; // Decrement debounce counter if reading is above threshold
        }
        if (debounceCounter <= -debounceThreshold) {
            // Change state only if consistent readings have been met
            middleSensorActive = false;
        }
    }
}

bool CupSet::getMiddleSensorState(){
    return middleSensorActive;
}

int CupSet::countCupsPresent(){
  int counter = 0;
  for (int i = 0; i < NUM_CUPS; i++){
    if(cups[i].isCupPresent()){
      counter+= 1;
    }
  }
  if(getSpecialState() == 6 || getSpecialState() == 7 || getSpecialState() == 3){
    counter+= 1;
  }
  return counter;
}

BeerPongState CupSet::getSpecialState(){
  return currentState;
}

Cup& CupSet::getSideCups(int index) {
    return SideCups[index]; 
}

void CupSet::initializeSideCups(int ledpin, int sensorpin, int ledpin2, int sensorpin2){
  SideCups[0].setLedPin(ledpin);
  SideCups[0].setSensorPin(sensorpin);
  SideCups[1].setLedPin(ledpin2);
  SideCups[1].setSensorPin(sensorpin2);
}

void CupSet::updateSideCupSensorState(int index, int value, String side){
    SideCups[index].UpdateSideCupSensors(value, index, side);
}

bool CupSet::areBothSideCupsActivated(){
 return bothSideCupsActivated;
}


bool CupSet::areBothSideCupsRemoved(){
  if(!this->getSideCups(0).isCupPresent() && !this->getSideCups(1).isCupPresent()){
    return true;
  }
  else{
    return false;
  }
}








