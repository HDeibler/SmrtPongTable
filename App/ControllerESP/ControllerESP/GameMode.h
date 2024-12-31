
#include <Arduino.h>

#ifndef GAMEMODE_H
#define GAMEMODE_H


class Table;

enum class GameModeType {
    BeerPong,
    BeerBall,
    SledBob,
};

class GameMode {
public:
    virtual ~GameMode() {}
    virtual GameModeType getType() const = 0;
    virtual void handleData(Table& table,  String& data, char side) = 0;
    virtual String createLEDCommands(Table& table) = 0;

};

#endif 
