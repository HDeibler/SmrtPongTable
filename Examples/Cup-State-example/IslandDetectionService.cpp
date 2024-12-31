#include "IslandDetectionService.h"
#include <HardwareSerial.h>
#include <Arduino.h>
// Constants
#define BACK_RIGHT_PIN 44
#define BACK_MIDDLE_PIN 9 
#define BACK_LEFT_PIN 2   
#define MID_RIGHT_PIN 1 
#define MID_LEFT_PIN 6   
#define FRONT_PIN 7


// Global Variables
unsigned long lastPrintMillis = 0;
const long printInterval = 2000; 
int GG[3][3];
int LED_PINS[3][3] = {
  {-1, FRONT_PIN, -1},
  {MID_LEFT_PIN, -1, MID_RIGHT_PIN},
  {BACK_LEFT_PIN, BACK_MIDDLE_PIN, BACK_RIGHT_PIN}
};
const char* CUP_NAMES[3][3] = {
    {"", "Front Cup", ""},
    {"Middle Left Cup", "", "Middle Right Cup"},
    {"Back Left Cup", "Back Middle Cup", "Back Right Cup"}
};
unsigned long previousMillis = 0;
const long interval = 5000; 
const long blinkInterval = 200;


void setupPins();
void readCupStates();
bool isIsland(int x, int y);
void checkIslands();

void setupIslandDetection() {
    setupPins();
}

void printCupStatus();  
void loopIslandDetection() {
    readCupStates();
    checkIslands();
    delay(1000 - 4*blinkInterval);  //To be removed

    unsigned long currentMillis = millis();
    if (currentMillis - lastPrintMillis >= printInterval) {
        printCupStatus();
        lastPrintMillis = currentMillis;
    }

    delay(1000 - 4*blinkInterval);  //To be removed
}


void printCupStatus() {
    Serial.println("Current cup status:");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (LED_PINS[i][j] != -1) {
                Serial.print(CUP_NAMES[i][j]);
                Serial.print(": ");
                Serial.println(GG[i][j] == 1 ? "Present" : "Absent");
            }
        }
    }
    Serial.println("---------------");
}




void setupPins() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (LED_PINS[i][j] != -1) {
                pinMode(LED_PINS[i][j], INPUT);
            }
        }
    }
}

void readCupStates() {
    int cupStates[3][3] = {
        {-1, digitalRead(FRONT_PIN), -1},
        {digitalRead(MID_LEFT_PIN), -1, digitalRead(MID_RIGHT_PIN)},
        {digitalRead(BACK_LEFT_PIN), digitalRead(BACK_MIDDLE_PIN), digitalRead(BACK_RIGHT_PIN)}
    };
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            GG[i][j] = (cupStates[i][j] == HIGH) ? 1 : 0;
        }
    }
}




bool isIsland(int x, int y) {
    if (GG[x][y] == 0) return false;
    int neighbors[8][2] = {{x-1,y},{x+1,y},{x,y-1},{x,y+1},{x-1,y-1},{x+1,y-1},{x-1,y+1},{x+1,y+1}};
    for (int i = 0; i < 8; i++) {
        int nx = neighbors[i][0];
        int ny = neighbors[i][1];
        if (nx >= 0 && ny >= 0 && nx < 3 && ny < 3 && GG[nx][ny] == 1) {
            return false;
        }
    }
    return true;
}

void checkIslands() {
    bool islandDetected = false;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (LED_PINS[i][j] == -1) continue;
            if (isIsland(i, j)) {
                islandDetected = true;
                Serial.print("Island at: ");
                Serial.println(CUP_NAMES[i][j]);
            }
        }
    }
}


