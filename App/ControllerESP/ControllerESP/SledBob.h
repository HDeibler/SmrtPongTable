#ifndef SLEDBOB_H
#define SLEDBOB_H

#include "GameMode.h"
#include "Table.h"
#include "Config.h"
#include <array>

class SledBob : public GameMode {
private:
  bool roundEnded = false;
public:
    int left = 0;
    int right = 0;
    GameModeType getType() const override {
      return GameModeType::SledBob;
    }
    void handleData(Table& table, String& data, char side) override;
    String createLEDCommands(Table& table) override;
    void updateGameLogic(Table& table, String side);
    String generateCommandsForCupSet(Table& table, String side);
    void updateCupPresenceStatus(Table& table, String side);
    void endRoundAnimations(Table& table, String winningSide);
    void pulseSideCups(Table& table);
};

#endif
