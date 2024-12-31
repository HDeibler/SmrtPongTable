#ifndef TABLE_H
#define TABLE_H

#include "CupSet.h"
#include "GameMode.h"
#include "LedMatrix.h"
#include "Cup.h" 
#include <Arduino.h>
#include "Config.h"

class Table {
private:
    CupSet leftSet;
    CupSet rightSet;
    GameMode* currentGameMode;
    LedMatrix matrix;
    static const int NUM_CUPS = 4;


public:
    
    Table();
    void setCurrentGameMode(GameMode* mode);
    void updateCurrentGame(Table& table, String& data, char side);
    String getCurrentGameMode(Table& table);
    String generateLEDCommands();
    void updateCupSet(const int sensorValues[], char side);
    Cup& getCup(int index, const String& side);
    void printLeftCupStates();
    void printRightCupStates();
    CupSet& getCupSet(const String& side);
    CupStatus checkCupStatus(int index, const String& side);
    void updateMiddleSensorState(int sensorValue, char side);
    bool getMiddleSensorState(const String& side);
    void updateDisplay();
    void clearLeds();
    void startUp();
    void bootUp();
};

#endif 
