#include "SledBob.h"
#include "Config.h"


void SledBob::handleData(Table& table, String& data, char sideChar) {
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

    try {
      int sensorValues[6]; 
      int index = 0;
      int irSensorValue = 0;


      int irIndex = data.indexOf("IR:");
      if (irIndex != -1) {
          String irSensorData = data.substring(irIndex + 3);
          irSensorValue = irSensorData.toInt();
          data = data.substring(0, irIndex - 1);
      }

      for (int i = 0; i < data.length(); i++) {
          int sensorStart = i;
          while (i < data.length() && data[i] != ',') i++;
          sensorValues[index++] = data.substring(sensorStart, i).toInt();
      }
      if (side == "Left") {
          sensorValues[2] += -20;
      }

      table.updateCupSet(sensorValues, sideChar);
      if(roundEnded){
          endRoundAnimations(table, side);
          return;
      }else{
        updateGameLogic(table, side);
      }
      
    } catch (const std::exception& e) {
        Serial.print("Exception caught in handleData: ");
        Serial.println(e.what());
    } catch (...) {
        Serial.println("Unknown exception caught in handleData");
    }


    
}

void SledBob::updateGameLogic(Table& table, String side) {
  try {
    CupSet& cupSet = table.getCupSet(side);
    unsigned long currentTime = millis(); 

    for (int i = 0; i < 2; ++i) {
        Cup& cup = cupSet.getSideCups(i);
        bool isPresent = cup.isCupPresent();

        if (isPresent && !cup.getActivated()) {
            cup.setActivated(true);
            cup.setActivationTime(currentTime);
        }
        unsigned long elapsedTime = currentTime - cup.getActivationTime();

        if (elapsedTime > 2000 && cup.getActivated()) {
              cup.setActivated(true);
              fill_solid(cup.getLedStrip(), 16, CRGB::Gold);
          if(cupSet.getSideCups(0).getActivated() && cupSet.getSideCups(1).getActivated()){
             unsigned long time = currentTime - cupSet.getSideCups(1).getActivationTime();
             unsigned long time1 = currentTime - cupSet.getSideCups(1).getActivationTime();
              if(time > 2000 && time1 > 2000){
                cupSet.setBothSideCupsActivated(true);
                updateCupPresenceStatus(table, side);
                break;
              }
              
          }
        }
        else if (isPresent) {
            int numLedsToLight = map(elapsedTime, 0, 2000, 0, 16); // Map 2 seconds to the total number of LEDs
            numLedsToLight = constrain(numLedsToLight, 0, 16); // Ensure the value is within bounds

            // Light up the LEDs progressively
            for (int ledIndex = 0; ledIndex < numLedsToLight; ledIndex++) {
                cup.getLedStrip()[ledIndex] = CRGB::Gold;
            }
        } else {
            // If the cup is not present, ensure it's not marked as activated
            cup.setActivated(false);
            cup.setActivationTime(currentTime);
            fill_solid(cup.getLedStrip(), 16, CRGB::Black);
        }
    }
      FastLED.show(); 
     
  }catch (const std::exception& e) {
    Serial.print("Exception caught in updateGameLogic: ");
    Serial.println(e.what());
  } catch (...) {
    Serial.println("Unknown exception caught in updateGameLogic");
  }
}

void SledBob::updateCupPresenceStatus(Table& table, String side) {
    try {
        CupSet& cupSet = table.getCupSet(side);
        if (!cupSet.areBothSideCupsActivated()) return;

        bool anyCupRemoved = false;  // Flag to check if any cup is marked as removed

        for (int i = 0; i < cupSet.getNumCups(); ++i) {
            Cup& cup = cupSet.getCup(i);
            bool isPresent = cup.isCupPresent();

            // Check only for cups that were previously active (present) and now are not present.
            if (!isPresent && cup.wasActive()) {
                unsigned long elapsedNotPresentTime = cup.getTimeSinceLastActive();
                if (elapsedNotPresentTime > 2000) {
                    // This cup was present, now it's not, and it's been so for more than 5 seconds.
                    cup.markAsRemoved();
                    anyCupRemoved = true;
                    Serial.print(cup.getName());
                    Serial.println(" marked as removed.");
                    // No need to call setRecentlyActive(false) here since we want to remember it was active for this logic to work.
                }
            } else if (isPresent) {
                // If the cup is currently present, ensure it's marked as recently active.
                cup.setRecentlyActive(true);
            }
        }

        if (anyCupRemoved) {
            // A cup that was present has been removed, which may affect the game state.
            roundEnded = true;
            cupSet.setWinner(true);
            endRoundAnimations(table, side);
        }
    } catch (const std::exception& e) {
        Serial.print("Exception caught in updateCupPresenceStatus: ");
        Serial.println(e.what());
    } catch (...) {
        Serial.println("Unknown exception caught in updateCupPresenceStatus");
    }
}

void SledBob::endRoundAnimations(Table& table, String side){
  pulseSideCups(table);

  if (table.getCupSet("Left").areBothSideCupsRemoved() && table.getCupSet("Right").areBothSideCupsRemoved()) {
          table.getCupSet("Left").setBothSideCupsActivated(false);
          table.getCupSet("Right").setBothSideCupsActivated(false);

          table.getCupSet("Right").getSideCups(0).setActivated(false);
          table.getCupSet("Right").getSideCups(1).setActivated(false);
          table.getCupSet("Left").getSideCups(0).setActivated(false);
          table.getCupSet("Left").getSideCups(1).setActivated(false);

          table.getCupSet("Left").setWinner(false);
          table.getCupSet("Right").setWinner(false);

          for(int i = 0; i < 6; i++){
          table.getCupSet("Right").getCup(i).resetTimeSinceLastActive();
          table.getCupSet("Left").getCup(i).resetTimeSinceLastActive();
          
          roundEnded = false;

          }
      }
  }

void SledBob::pulseSideCups(Table& table) {
    CRGB winningColor = CRGB::Green;
    CRGB losingColor = CRGB::Red;
    
    for (int i = 0; i < 2; ++i) {
      CRGB color = (table.getCupSet("Left").isWinner() ? winningColor : losingColor);
      fill_solid(table.getCupSet("Left").getSideCups(i).getLedStrip(), 16, color); 
    }

    for (int i = 0; i < 2; ++i) { 
      CRGB color = (table.getCupSet("Right").isWinner() ? winningColor : losingColor);
      fill_solid(table.getCupSet("Right").getSideCups(i).getLedStrip(), 16, color); 
    }
}


String SledBob::createLEDCommands(Table& pongTable) {
  if(pongTable.getCupSet("Left").isWinner()){
    return "Winner|Loser";
  }
  else if(pongTable.getCupSet("Right").isWinner()){
    return "Loser|Winner";
  }
  try{
    String leftCommand = "";
    String rightCommand = "";
    String ledCommand = "";
    // If the left side cups are activated, generate commands for the right cup set
    if (pongTable.getCupSet("Left").areBothSideCupsActivated()) {
        leftCommand = generateCommandsForCupSet(pongTable, "Left");
    }
    // If the right side cups are activated, generate commands for the left cup set
    if (pongTable.getCupSet("Right").areBothSideCupsActivated()) {
        rightCommand = generateCommandsForCupSet(pongTable, "Right");
    }


    if (!leftCommand.isEmpty() && !rightCommand.isEmpty()) {

        ledCommand = leftCommand + "|" + rightCommand;
    } else if (!leftCommand.isEmpty()) {

        ledCommand = leftCommand + "|-1";
    } else if (!rightCommand.isEmpty()) {

        ledCommand =  "-1|" + rightCommand;
    }
    else{
      ledCommand = "-1|-1";
    }
    return ledCommand;
  }catch (const std::exception& e) {
    Serial.print("Exception caught in updateGameLogic: ");
    Serial.println(e.what());
    return "-1|-1";
  } catch (...) {
    Serial.println("Unknown exception caught in updateGameLogic");
    return "-1|-1";
  }
}


String SledBob::generateCommandsForCupSet(Table& table, String side) {
    CupSet& cupSet = table.getCupSet(side);
    String command = "";

    for (int i = 0; i < cupSet.getNumCups(); i++) {
        command += "P" + String(i+1) + ":B" + (cupSet.getCup(i).isCupPresent() ? "1111" : "0000");
        if (i < cupSet.getNumCups() - 1) {
            command += ",";
        }
    }

    return command;
}





  