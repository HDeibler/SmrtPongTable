#ifndef CUPSET_H
#define CUP_SET_H


#include "Cup.h"
#include "BeerPongStates.h"
#include "Config.h"

class CupSet {
private:
    static const int NUM_CUPS = 6;
    Cup cups[NUM_CUPS];
    String side;  
    bool middleSensorActive; 
    int debounceCounter;
    BeerPongState currentState;
    Cup SideCups[2];
    CRGB leds[NUM_LEDS_PER_STRIP];
    bool bothSideCupsActivated = false;
    bool winner = false;

public:
    CupSet(String sideLabel);
    void updateState(const int sensorValues[], char side);
    Cup& getCup(int index);
    String getSide();
    int getNumCups() const;
    static int getNumCupsConstant();
    void updateMiddleSensorState(int sensorValue);
    bool getMiddleSensorState();
    int countCupsPresent();
    void setSpecialState(BeerPongState state);
    BeerPongState getSpecialState();
    void initializeSideCups(int ledpin, int sensorpin, int ledpin2, int sensorpin2);
    void updateSideCupSensorState(int index, int value, String side);
    Cup& getSideCups(int i);
    bool areBothSideCupsActivated();
    void setBothSideCupsActivated(bool status) {bothSideCupsActivated = status; }
    bool areBothSideCupsRemoved();
    bool isWinner() {return winner;}
    void setWinner(bool champ) {winner = champ;}

};

#endif 