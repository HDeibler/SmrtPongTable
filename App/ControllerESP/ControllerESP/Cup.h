#ifndef CUP_H
#define CUP_H

#include <Arduino.h>
#include "Config.h"
static constexpr int NUM_LEDS_PER_CUP = 16;

extern CRGB leds1[NUM_LEDS_PER_CUP];
extern CRGB leds2[NUM_LEDS_PER_CUP];
extern CRGB leds3[NUM_LEDS_PER_CUP];
extern CRGB leds4[NUM_LEDS_PER_CUP];
extern CRGB* ledsArray[];


enum CupStatus {
    Regular,
    Island,
    Bermuda,
    WestVirginia,
    LastCup,
    RedLight, 
    CupAndMiddleSensorActive,
};

class Cup {
private:
    String name;
    bool isPresent;
    int debounceCounter;
    bool isIsland;
    bool isBermuda;
    bool isWestVirginia;
    bool isLast;
    bool isRedLight;
    bool isCupAndMiddleSensorActive;
    float filteredValue; 
    static constexpr float alpha = 0.1;
    int sensorPin; 
    int ledPin;    
    int ledStripIndex;
    bool activated = false;
    unsigned long activationTime; 
    unsigned long lastActiveTime;
    bool wasRecentlyActive = false;
    bool isPermanentlyRemoved = false;


public:
  //tsk tsk *messy messy messy* I should create a dedicated Led class driver
    bool _isAnimating = false;
    Cup(String cupName);
    Cup();
    String getName();
    bool isCupPresent();
    void updateState(int sensorValue, char side, int index);
    void setIsland(bool status);
    void setBermuda(bool status);
    bool isIslandCup();
    bool isBermudaCup();
    void setWestVirginia(bool status);
    bool isWestVirginiaCup();
    void setLastCup(bool status);
    bool isLastCup();
    bool isRedLightCup();
    void setRedLight(bool status);
    void setCupAndMiddleSensorActive(bool status) {isCupAndMiddleSensorActive = status; }
    bool isCupAndMiddleSensorActiveCup();
    void UpdateSideCupSensors(int sensorValue, int index, String side);
    void setActivated(bool status) {activated = status; }
    bool getActivated() {return activated; }
    void setLedStripIndex(int index) { ledStripIndex = index; }
    CRGB* getLedStrip() const { return ledsArray[ledStripIndex]; }
    void setSensorPin(int value) { sensorPin = value; }
    int getSensorPin() { return sensorPin; }
    int getLedPin() { return ledPin; }
    void setLedPin(int value) {ledPin = value; }
    bool usesSensor() const { return sensorPin != -1; }
    bool usesLed() const { return ledPin != -1; }
    void setActivationTime(unsigned long time) {activationTime = time; }
    unsigned long getActivationTime() const {return activationTime; } 
    void setRecentlyActive(bool isActive);
    bool wasActive();
    unsigned long getTimeSinceLastActive();
    void resetTimeSinceLastActive();
    void markAsRemoved();
    bool isRemoved();




};

#endif 
