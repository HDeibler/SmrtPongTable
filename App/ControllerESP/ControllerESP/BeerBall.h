#ifndef BEERBALL_H
#define BEERBALL_H

#include "GameMode.h"
#include "Table.h"
#include "Config.h"


class BeerBall : public GameMode {
private:
    static const int NUM_LEDS_PER_POD = 16;
    static const int NUM_PODS = 4;
public:
    GameModeType getType() const override {
      return GameModeType::BeerBall;
    }

    void handleData(Table& table, String& data, char side) override;
    String createLEDCommands(Table& table) override;
    void updateGameLogic(Table& table, String side);
    String generateCommandsForMiddleCup(Table& table, String side);

};

#endif
