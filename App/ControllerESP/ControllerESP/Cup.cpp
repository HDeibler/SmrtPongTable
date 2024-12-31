#include "Cup.h"


Cup::Cup(String cupName) 
    : name(cupName), isPresent(false), debounceCounter(0), 
      isIsland(false), isBermuda(false), isWestVirginia(false), isLast(false), isRedLight(false), isCupAndMiddleSensorActive(false),
      filteredValue(0.0), sensorPin(-1), ledPin(-1) {
   
}

String Cup::getName() {
    return name;
}

bool Cup::isCupPresent() {
    return isPresent;
}

void Cup::updateState(int sensorValue, char side, int index) {
    int threshold = 10; 
    const int debounceThreshold = 5;
    if(side == 'R' && index == 4){
      threshold = 10;
    }
        if(side == 'L' && index == 0){
      threshold = 3;
    }

    filteredValue = alpha * sensorValue + (1 - alpha) * filteredValue;

    if (filteredValue <= threshold) {
        if (debounceCounter < debounceThreshold) {
            debounceCounter++;
        } else if (debounceCounter == debounceThreshold) {
            isPresent = true;
        }
    } else {
        debounceCounter = 0;
        isPresent = false;
    }
}

// I had to do it this way ~ I had to do it this way ~ I had to do it this way ~ I had to do it this way  
// Don't try to change it ~ Don't try to change it ~ Don't try to change it :)
void Cup::setIsland(bool status) {
    isIsland = status;
}

void Cup::setBermuda(bool status) {
    isBermuda = status;
}

bool Cup::isIslandCup() {
    return isIsland;
}

bool Cup::isBermudaCup(){
    return isBermuda;
}

void Cup::setWestVirginia(bool status) {
    isWestVirginia = status;
}

bool Cup::isWestVirginiaCup() {
    return isWestVirginia;
}

void Cup::setLastCup(bool status) {
    isLast = status;
}

bool Cup::isLastCup() {
    return isLast;
}
bool Cup::isRedLightCup(){
    return isRedLight;
}

void Cup::setRedLight(bool status){
    isRedLight = status;
}

bool Cup::isCupAndMiddleSensorActiveCup(){
  return isCupAndMiddleSensorActive;
}


void Cup::UpdateSideCupSensors(int sensorValue, int index, String side){
    int threshold = 105; 
    const int debounceThreshold = 7;
    if(side == "Left" && index == 1){
      threshold = 190;
    }
    else if((side == "Left" && index == 0) || (side == "Right" && index == 1 )){
      threshold = 115;
    }


    if(side == "Right" && index == 0){
      threshold = 115;
    }
    filteredValue = alpha * sensorValue + (1 - alpha) * filteredValue;

    if (filteredValue <= threshold) {
        if (debounceCounter < debounceThreshold) {
            debounceCounter++;
        } else if (debounceCounter == debounceThreshold) {
            isPresent = true;
        }
    } else {
        debounceCounter = 0;
        isPresent = false;
    }
}
void Cup::setRecentlyActive(bool Active) {
    if (Active) {
        lastActiveTime = millis();
    }
    wasRecentlyActive = Active;
}

bool Cup::wasActive() {
    return wasRecentlyActive;
}

unsigned long Cup::getTimeSinceLastActive() {
    return millis() - lastActiveTime;
}

void Cup::resetTimeSinceLastActive() {
    lastActiveTime = millis();
    wasRecentlyActive = false;
    isPermanentlyRemoved = false;
}

void Cup::markAsRemoved() {
    isPermanentlyRemoved = true;
}

bool Cup::isRemoved() {
    return isPermanentlyRemoved;
}








    






