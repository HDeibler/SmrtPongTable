#include "BeerBall.h"
#include "Config.h"

void BeerBall::handleData(Table& table, String& data, char sideChar) {
  String side = (sideChar == 'L') ? "Left" : "Right";
  CupSet& cupSet = table.getCupSet(side);
  int sensorValueIndex = 4;
  int sensorValue = 0;
  int commaCount = 0;
  String sensorValueStr = "";
  int cupIndex = 4;

  for (int i = 0; i < 2; ++i) {
    int sensorValue = analogRead(table.getCupSet(side).getSideCups(i).getSensorPin());
    table.getCupSet(side).updateSideCupSensorState(i, sensorValue, side);
  }
  for (int i = 0; i < data.length(); ++i) {
    if (data[i] == ',') {
      commaCount++;
      continue;
    }
    if (commaCount == sensorValueIndex) {
      sensorValueStr += data[i];
    } else if (commaCount > sensorValueIndex) {
      break;
    }
  }
  if (sensorValueStr.length() > 0) {
    sensorValue = sensorValueStr.toInt();
  }
  table.getCupSet(side).getCup(cupIndex).updateState(sensorValue, sideChar, cupIndex);

  updateGameLogic(table, side);
}

void BeerBall::updateGameLogic(Table& table, String side) {
  CupSet& cupSet = table.getCupSet(side);

  for (int i = 0; i < 2; ++i) {
    Cup& cup = cupSet.getSideCups(i);
    bool isPresent = cup.isCupPresent();
    CRGB color = (side == "Left") ? CRGB::Blue : CRGB::Red;
    CRGB* ledStrip = cup.getLedStrip();
    if (isPresent) {
      fill_solid(ledStrip, 16, color);
    } else {
      fill_solid(ledStrip, 16, CRGB::Black);
    }
  }
  FastLED.show();
}

String BeerBall::createLEDCommands(Table& pongTable) {
  String leftCommands = generateCommandsForMiddleCup(pongTable, "Left");
  String rightCommands = generateCommandsForMiddleCup(pongTable, "Right");

  String ledCommands = "";

  if (!leftCommands.isEmpty() && !rightCommands.isEmpty()) {
    // Both sides have commands; use a delimiter in between
    ledCommands = leftCommands + "|" + rightCommands;
  } else if (!leftCommands.isEmpty()) {
    // Only the left side has commands; delimiter at the end if needed for future expansion
    ledCommands = leftCommands + "|";
  } else if (!rightCommands.isEmpty()) {
    // Only the right side has commands; delimiter at the start
    ledCommands = "|" + rightCommands;
  }

  return ledCommands;
}

String BeerBall::generateCommandsForMiddleCup(Table& table, String side) {
  int count = 0;
  String command = "";
  for (int i = 0; i < 2; ++i) {
    if (table.getCupSet(side).getSideCups(i).isCupPresent()) count++;
  }
  if (side == "Left" && count == 0 && table.getCupSet(side).getCup(4).isCupPresent()) {
    command = "BBB";
  } else if (side == "Right" && count == 0 && table.getCupSet(side).getCup(4).isCupPresent()) {
    command = "BBR";
  } else {
    command = "-1";
  }
  return command;
}
