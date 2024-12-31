#include "IslandDetectionService.h"


void setup() {
    Serial.begin(9600);

    // Setup for Island Detection
    setupIslandDetection();
    

}

void loop() {

    loopIslandDetection();
}
