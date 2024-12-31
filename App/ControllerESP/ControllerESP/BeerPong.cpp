#include "BeerPong.h"
#include "BeerPongStates.h"

void BeerPong::handleData(Table& table, String& data, char side) {
    try {
        int sensorValues[6]; 
        int index = 0;
        int irSensorValue = 0;

        // Extract IR sensor value if present
        int irIndex = data.indexOf("IR:");
        if (irIndex != -1) {
            String irSensorData = data.substring(irIndex + 3);
            irSensorValue = irSensorData.toInt();
          
            data = data.substring(0, irIndex - 1);
        }

        // Parse cup sensor values
        for (int i = 0; i < data.length(); i++) {
            int sensorStart = i;
            while (i < data.length() && data[i] != ',') i++;
            sensorValues[index++] = data.substring(sensorStart, i).toInt();
        }
        if (side == 'L') {
            sensorValues[2] += -20;
        }

        table.updateCupSet(sensorValues, side);
        table.updateMiddleSensorState(irSensorValue, side);
        applyGameLogic(table);
    } catch (const std::exception& e) {
        // Log exception details, proceed with any cleanup or fallback. Serial is messy messy messy :(
        Serial.print("Exception caught in handleData: ");
        Serial.println(e.what());
    } catch (...) {
        Serial.println("Unknown exception caught in handleData");
    }
}


void BeerPong::applyGameLogic(Table &table) {

    String islandStateLeft = checkIslands(table, "Left");
    String islandStateRight = checkIslands(table, "Right");

    bool westVirginiaLeft = checkWestVirginia(table, "Left");
    bool westVirginiaRight = checkWestVirginia(table, "Right");

    bool redLightLeft = checkRedLightCondition(table, "Left");
    bool redLightRight = checkRedLightCondition(table, "Right");

    bool lastCupLeft = checkLastCup(table, "Left");
    bool lastCupRight = checkLastCup(table, "Right");

    bool middleSensorAndCupLeft = checkMiddleSensorWithSingleCupCondition(table, "Left");
    bool middleSensorAndCupRight = checkMiddleSensorWithSingleCupCondition(table, "Right");

    bool middleSensorOnlyConditionLeft = checkMiddleSensorOnlyCondition(table, "Left");
    bool middleSensorOnlyConditionRight = checkMiddleSensorOnlyCondition(table, "Right");
    

    updateCupSetState(table.getCupSet("Left"), islandStateLeft, westVirginiaLeft, redLightLeft, lastCupLeft, middleSensorOnlyConditionLeft, middleSensorAndCupLeft);
    updateCupSetState(table.getCupSet("Right"), islandStateRight, westVirginiaRight, redLightRight, lastCupRight, middleSensorOnlyConditionRight, middleSensorAndCupRight);

}

void BeerPong::updateCupSetState(CupSet& cupSet, const String& islandState, bool westVirginia, bool redLight, bool lastCup, bool middleSensorIsLast, bool middleSensorAndCup) {
    // Check the island state first
    if (islandState == "Bermuda") {
        cupSet.setSpecialState(BeerPongState::CS_Bermuda);
    } else if (islandState == "Island") {
        cupSet.setSpecialState(BeerPongState::CS_Island);
    } else if (westVirginia) {
        cupSet.setSpecialState(BeerPongState::CS_WestVirginia);
    } else if (redLight) {
        cupSet.setSpecialState(BeerPongState::CS_RedLight);
    } else if (lastCup) {
        cupSet.setSpecialState(BeerPongState::CS_LastCup);   
    } else if (middleSensorIsLast){
        cupSet.setSpecialState(BeerPongState::CS_MiddleSensorOnly);
    }
      else if(middleSensorAndCup){
        cupSet.setSpecialState(BeerPongState::CS_MiddleSensorWithSingleCup);
      }
    
    else {
        cupSet.setSpecialState(BeerPongState::CS_Regular);
    }
}


bool BeerPong::isIsland(Table & table, int index,
  const String & side) {

  int cupCount = 0;
  for (int i = 0; i < 6; ++i) { 
    if (table.getCup(i, side).isCupPresent()) {
      cupCount++;
    }
  }
  if (cupCount > 4) {
    return false;
  }

  else if (cupCount < 3) {
    return false;
  }

  int pyramid[3][3] = {
    // -1's are blank space, maybe there is a library for 2d traingle arrays :-)
    {-1,0,-1},
    {1,-1,2},
    {3,4,5}
  };

  int x, y;
  bool found = false;
  for (x = 0; x < 3; ++x) {
    for (y = 0; y < 3; ++y) {
      if (pyramid[x][y] == index) {
        found = true;
        break;
      }
    }
    if (found) break;
  }

  // Check if the cup is present
  if (!table.getCup(index, side).isCupPresent()) return false; // Cup itself is not present

  // Special case for cups 1 and 2 (middle row)
  if (index == 1 || index == 2) {
    // If both cups are present, neither is an island
    if (table.getCup(1, side).isCupPresent() && table.getCup(2, side).isCupPresent()) {
      return false;
    }
  }

  // Check for neighboring cups
  for (int dx = -1; dx <= 1; ++dx) {
    for (int dy = -1; dy <= 1; ++dy) {
      int nx = x + dx;
      int ny = y + dy;
      if (nx >= 0 && nx < 3 && ny >= 0 && ny < 3 && pyramid[nx][ny] != -1 && pyramid[nx][ny] != index) {
        if (table.getCup(pyramid[nx][ny], side).isCupPresent()) return false;
      }
    }
  }
  return true; // No neighbors present, it's an island
}


String BeerPong::checkIslands(Table &table, const String &side) {
    int cupCount = 0;
    int islandCount = 0;

    // Count the number of cups present and identify island cups
    for (int i = 0; i < 6; ++i) {
        if (table.getCup(i, side).isCupPresent()) {
            cupCount++;
            if (isIsland(table, i, side)) {
                islandCount++;
            }
        }
    }


    if (islandCount == 3) {
        // We know if there are three islands, its actual a bermuda formation
        // Set Bermuda and reset island status for specific cups 
        for (int i : {0, 3, 5}) {
            table.getCup(i, side).setIsland(false);
            table.getCup(i, side).setBermuda(true);
        }
        return "Bermuda";
    } else {

        bool anyIslands = false;
        for (int i = 0; i < 6; ++i) {
            if (table.getCup(i, side).isBermudaCup()) {
                table.getCup(i, side).setBermuda(false);
            }
            if (isIsland(table, i, side)) {
                table.getCup(i, side).setIsland(true);
                anyIslands = true;
            } else {
                table.getCup(i, side).setIsland(false);
            }
        }
        return anyIslands ? "Island" : "None";
    }
}

bool BeerPong::checkWestVirginia(Table & table,
  const String & side) {
  // Ensure the front cup is absent
  bool frontCupAbsent = !table.getCup(0, side).isCupPresent();

  // Ensure the back row of cups is present
  bool backCupsPresent = table.getCup(3, side).isCupPresent() &&
    table.getCup(4, side).isCupPresent() &&
    table.getCup(5, side).isCupPresent();

  // Check that exactly one middle cup is present
  bool onlyOneMiddleCup = (table.getCup(1, side).isCupPresent() && !table.getCup(2, side).isCupPresent()) ||
    (!table.getCup(1, side).isCupPresent() && table.getCup(2, side).isCupPresent());


  bool isWestVirginia = frontCupAbsent && backCupsPresent && onlyOneMiddleCup;

  for (int i = 0; i < 6; ++i) {
    table.getCup(i, side).setWestVirginia(false);
  }

  if (isWestVirginia) {
    table.getCup(1, side).setWestVirginia(table.getCup(1, side).isCupPresent());
    table.getCup(2, side).setWestVirginia(table.getCup(2, side).isCupPresent());
    for (int i = 3; i <= 5; ++i) { // Back row
      table.getCup(i, side).setWestVirginia(true);
    }
  }

  return isWestVirginia;
}

bool BeerPong::checkLastCup(Table & table, const String & side) {
    int cupsPresent = 0;
    int lastCupIndex = -1;

    for (int i = 0; i < 6; ++i) {
        if (table.getCup(i, side).isCupPresent()) {
            cupsPresent++;
            lastCupIndex = i;
        }
    }
    // Return true if there's exactly one cup left, otherwise false
    return (cupsPresent == 1 && lastCupIndex != -1) && (table.getCupSet(side).getMiddleSensorState() == false);
}


bool BeerPong::checkRedLightCondition(Table& table, const String& side) {
    // Check if the middle sensor is active
    bool middleSensorActive = table.getMiddleSensorState(side);

    // Check if the front cup (index 0) and back middle cup (index 4) are present
    bool frontCupPresent = table.getCup(0, side).isCupPresent();
    bool backMiddleCupPresent = table.getCup(4, side).isCupPresent();

    // Ensure that only the front cup and back middle cup are present
    bool onlySpecificCupsPresent = true;
    for (int i = 1; i < 6; i++) { 
        if (i != 4 && table.getCup(i, side).isCupPresent()) { 
            onlySpecificCupsPresent = false;
            break;
        }
    }
    bool redLightConditionMet = middleSensorActive && frontCupPresent && backMiddleCupPresent && onlySpecificCupsPresent;
    // Update the RedLight status based on the condition
    table.getCup(0, side).setRedLight(redLightConditionMet);
    table.getCup(4, side).setRedLight(redLightConditionMet);

    return redLightConditionMet; // Return true if the condition is met, false otherwise
}


String BeerPong::createLEDCommands(Table& pongTable){
    String leftCommands = generateCommandsForSet(pongTable, "Left");
    String rightCommands = generateCommandsForSet(pongTable, "Right");
    // Initialize ledCommands as an empty string
    String ledCommands = "";


    if (!leftCommands.isEmpty() && !rightCommands.isEmpty()) {
        ledCommands = leftCommands + "|" + rightCommands;
    } else if (!leftCommands.isEmpty()) {
        ledCommands = leftCommands + "|";
    } else if (!rightCommands.isEmpty()) {
        ledCommands = "|" + rightCommands;
    }
    return ledCommands;
}

String BeerPong::generateCommandsForSet(Table& pongTable, const String& side) {
    String commands = "";
    int cupsPresent = 0;
    int lastCupIndex = -1;
    bool isLastCupSingle = false;
    BeerPongState state = pongTable.getCupSet(side).getSpecialState();


    for (int i = 0; i < pongTable.getCupSet(side).getNumCups(); ++i) {
        if (pongTable.getCup(i, side).isCupPresent()) {
            cupsPresent++;
            lastCupIndex = i;
        }
    }

    if (pongTable.getMiddleSensorState(side) && cupsPresent == 0) {
      return "LS"; 
    }
    else if(cupsPresent == 1){
      isLastCupSingle = true;
    }
    else if (cupsPresent == 0) {
      return "-1";
    }

    for (int i = 0; i < pongTable.getCupSet(side).getNumCups(); ++i) {
        CupStatus status = pongTable.checkCupStatus(i, side);
        String colorCode = determineColorCode(status, pongTable, i, side, isLastCupSingle, lastCupIndex);
        
        commands += "P" + String(i + 1) + ":" + colorCode + (pongTable.getCup(i, side).isCupPresent() ? "1111" : "0000");
        if (i < pongTable.getCupSet(side).getNumCups() - 1) {
            commands += ",";
        }
    }


    if (state == CS_MiddleSensorWithSingleCup) {
      commands += ",IR"; 
    }

    return commands;
}


int BeerPong::countCupsPresent(Table& table, const String& side) {
    int cupsPresent = 0;
    for (int i = 0; i < 6; i++) {
        if (table.getCup(i, side).isCupPresent()) {
            cupsPresent++;
        }
    }
    return cupsPresent;
}

bool BeerPong::checkMiddleSensorOnlyCondition(Table& table, const String& side) {
    bool middleSensorActive = table.getMiddleSensorState(side);

    
    int cupsPresent = countCupsPresent(table, side);
    return (cupsPresent == 0 && middleSensorActive);
}

bool BeerPong::checkMiddleSensorWithSingleCupCondition(Table& table, const String& side) {
    bool middleSensorActive = table.getMiddleSensorState(side);
    int cupsPresent = 0;
    int singleCupIndex = -1;


    for (int i = 0; i < 6; ++i) {
        if (table.getCup(i, side).isCupPresent()) {
            cupsPresent++;
            singleCupIndex = i;
        }
    }
    // Condition is met if exactly one cup is present and the middle sensor is active.
    bool conditionMet = (cupsPresent == 1 && middleSensorActive);

    table.getCup(singleCupIndex, side).setCupAndMiddleSensorActive(conditionMet);
    return conditionMet;
}


String BeerPong::determineColorCode(CupStatus status, Table& pongTable, int cupIndex, const String& side, bool isLastCupSingle, int lastCupIndex) {
    String colorCode = "B"; 

    if (isLastCupSingle && cupIndex == lastCupIndex && pongTable.getMiddleSensorState(side) == false) {
        colorCode = "L";
    } else {
        switch (status) {
            case Bermuda:
                colorCode = "T";
                break;
            case Island:
                colorCode = "G";
                break;
            case CupAndMiddleSensorActive:
                colorCode = "MS";
                break;
            case WestVirginia:
                if (cupIndex == 1 && pongTable.getCup(1, side).isWestVirginiaCup()) {
                    colorCode = "WL"; // Left middle cup is part of West Virginia
                } else if (cupIndex == 2 && pongTable.getCup(2, side).isWestVirginiaCup()) {
                    colorCode = "WR"; // Right middle cup is part of West Virginia
                } else {
                    colorCode = "W"; // Back cups in West Virginia formation
                }
                break;
          case RedLight:
              colorCode = "R";
              break;
            case Regular:
            default:
                colorCode = "B"; // Default case for any cup not fitting above conditions
                break;
        }
    }

    return colorCode;
}





