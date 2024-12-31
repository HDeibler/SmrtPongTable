#include <Arduino.h>
#ifndef BEERPONG_H
#define BEERPONG_H

#include "GameMode.h"
#include "Table.h"

class BeerPong : public GameMode {
public:
    void handleData(Table& table, String& data, char side) override;
    GameModeType getType() const override {
          return GameModeType::BeerPong;
      }
private:
    void applyGameLogic(Table &table);
    bool isIsland(Table &table, int index, const String &side);
    String checkIslands(Table &table, const String &side);
    bool checkWestVirginia(Table &table, const String &side);
    bool checkLastCup(Table &table, const String &side);
    void updateCupSetState(CupSet& cupSet, const String& islandState, bool westVirginia, bool redLight, bool lastCup, bool middleSensorIsLast, bool middleSensorAndCup);
    bool checkRedLightCondition(Table &table, const String &side);
    String createLEDCommands(Table& pongTable);
    String generateCommandsForSet(Table& pongTable, const String& side);
    String determineColorCode(CupStatus status, Table& pongTable, int cupIndex, const String& side, bool isLastCupSingle, int lastCupIndex);
    bool checkMiddleSensorActivation(Table& table, const String& side);
    bool checkMiddleSensorOnlyCondition(Table& table, const String& side);
    bool checkMiddleSensorWithSingleCupCondition(Table& table, const String& side);
    int countCupsPresent(Table& table, const String& side);
};

#endif 
