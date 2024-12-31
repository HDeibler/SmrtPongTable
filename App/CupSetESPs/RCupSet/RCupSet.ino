/*Created by Hunter Deibler*/
/***********************************
****************RIGHT***************/

#include <FastLED.h>
#include "WiFi.h"
#include "esp_bt.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <HardwareSerial.h>
#include "esp_task_wdt.h"
#include <cmath>

#define RXp2 16
#define TXp2 17
#define IR_FILTER_WINDOW_SIZE 5
HardwareSerial MySerial2(2);

// Right ESP32 
#define NUM_LEDS_PER_POD 4
#define NUM_PODS 6
#define SENSOR_DATA_BUFFER_SIZE 50 



#define CRGB_Burgundy CRGB(214, 9, 0)
unsigned long lastDimTime = 0;
const unsigned long dimInterval = 1000; // Dim every 5000 milliseconds
uint8_t currentBrightness = 255; // Start at full brightness
const uint8_t minBrightness = 235; // Minimum brightness level to dim to
const uint8_t dimStep = 5; // Amount to dim each step
bool isDimming = false;


#define COMMAND_BUFFER_SIZE 256
char commandBuffer[COMMAND_BUFFER_SIZE];
int commandBufferIndex = 0;
bool isStartupAnimationRunning = false;


#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 0
// Global variables for animation

uint8_t animationBrightness = MAX_BRIGHTNESS; 


int brightness = 0; // Current brightness level
bool increasing = true; // Direction of change


//Different pins then left side
const int sensorPins[NUM_PODS] = {
  13,
  14,
  27,
  26,
  33,
  25
};
const int ledPins[NUM_PODS] = {
  32,
  23,
  22,
  18,
  19,
  21
};
#define IR_SENSOR_PIN 36

struct Pod {
  int sensorPin;
  int ledPin;
  CRGB leds[NUM_LEDS_PER_POD];
};

Pod pods[NUM_PODS];

void setupPods() {
  // Set up Pod 1
  FastLED.addLeds < NEOPIXEL, 32 > (pods[0].leds, NUM_LEDS_PER_POD);
  fill_solid(pods[0].leds, NUM_LEDS_PER_POD, CRGB::Black);
  pods[0].sensorPin = sensorPins[0];
  pods[0].ledPin = 32;

  // Set up Pod 2
  FastLED.addLeds < NEOPIXEL, 23 > (pods[1].leds, NUM_LEDS_PER_POD);
  fill_solid(pods[1].leds, NUM_LEDS_PER_POD, CRGB::Black);
  pods[1].sensorPin = sensorPins[1];
  pods[1].ledPin = 23;

  // Set up Pod 3
  FastLED.addLeds < NEOPIXEL, 22 > (pods[2].leds, NUM_LEDS_PER_POD);
  fill_solid(pods[2].leds, NUM_LEDS_PER_POD, CRGB::Black);
  pods[2].sensorPin = sensorPins[2];
  pods[2].ledPin = 22;

  // Set up Pod 4
  FastLED.addLeds < NEOPIXEL, 18 > (pods[3].leds, NUM_LEDS_PER_POD);
  fill_solid(pods[3].leds, NUM_LEDS_PER_POD, CRGB::Black);
  pods[3].sensorPin = sensorPins[3];
  pods[3].ledPin = 18;

  // Set up Pod 5
  FastLED.addLeds < NEOPIXEL, 19 > (pods[4].leds, NUM_LEDS_PER_POD);
  fill_solid(pods[4].leds, NUM_LEDS_PER_POD, CRGB::Black);
  pods[4].sensorPin = sensorPins[4];
  pods[4].ledPin = 19;

  // Set up Pod 6
  FastLED.addLeds < NEOPIXEL, 21 > (pods[5].leds, NUM_LEDS_PER_POD);
  fill_solid(pods[5].leds, NUM_LEDS_PER_POD, CRGB::Black);
  pods[5].sensorPin = sensorPins[5];
  pods[5].ledPin = 21;
}
static uint8_t rainbowHue = 0;

CRGB fireColors[] = {
    CRGB::Red, CRGB::Maroon, CRGB::DarkRed, CRGB::DarkRed,
    CRGB::Maroon, CRGB_Burgundy, CRGB::DarkRed, CRGB::Maroon,
    CRGB::Red, CRGB::DarkRed, CRGB_Burgundy, CRGB::DarkRed,
    CRGB::Maroon, CRGB_Burgundy, CRGB::DarkRed, CRGB::Maroon
};

// Function to shuffle the colors in the array
void shufflePalette(CRGB *palette, int size) {
    for (int i = 0; i < size - 1; i++) {
        int j = random(i, size);
        if (i != j) {
            // Swap elements
            CRGB temp = palette[i];
            palette[i] = palette[j];
            palette[j] = temp;
        }
    }
}

CRGBPalette16 firePalette = CRGBPalette16(
    CRGB::DarkRed, CRGB_Burgundy, CRGB::DarkRed, CRGB::DarkRed,
    CRGB::Maroon, CRGB::DarkRed, CRGB::DarkRed, CRGB::Maroon,
    CRGB::Red, CRGB::DarkRed, CRGB::DarkRed, CRGB::DarkRed,
    CRGB::Maroon, CRGB_Burgundy, CRGB::DarkRed, CRGB::Maroon);

void handleTCupPattern() {

    static uint16_t paletteShift = 0;
    int numPodsToDim = random(1, NUM_PODS + 1); // Random number of pods to dim

    // Create an array to track which pods should dim
    bool podsToDim[NUM_PODS] = {false};

    // Randomly choose pods to dim
    for (int i = 0; i < numPodsToDim; ++i) {
        int podIndex;
        do {
            podIndex = random(0, NUM_PODS);
        } while (podsToDim[podIndex]); // Ensure unique pods are selected for dimming
        podsToDim[podIndex] = true;
    }

    // Apply the dimming effect based on the selected pods
    for (int i = 0; i < NUM_PODS; ++i) {
        for (int j = 0; j < NUM_LEDS_PER_POD; ++j) {
            uint8_t colorIndex = (paletteShift + (uint16_t)(j * 4000.0 / NUM_LEDS_PER_POD)) % 255;
            CRGB color = ColorFromPalette(firePalette, colorIndex, 255, LINEARBLEND);

            // Dim the selected pods
            if (podsToDim[i]) {
                // Dim these pods to a random brightness between minBrightness and currentBrightness
                uint8_t dimBrightness = random(minBrightness, currentBrightness + 1);
                pods[i].leds[j] = color.nscale8(dimBrightness);
            } else {
                // Other pods remain at full brightness
                pods[i].leds[j] = color;
            }
        }
    }
    paletteShift += 1;
}

CRGBPalette16 westVirginiaPalette = CRGBPalette16(
    CRGB::Navy, CRGB::DarkBlue, CRGB::Blue, CRGB::Gold,
    CRGB::Gold, CRGB::Yellow, CRGB::Gold, CRGB::DarkOrange,
    CRGB::DarkBlue, CRGB::Navy, CRGB::DarkBlue, CRGB::Blue,
    CRGB::Gold, CRGB::Yellow, CRGB::Gold, CRGB::DarkOrange);



void handleWestVirginiaCommand(const String& command, bool isLeft) {
    static uint16_t paletteShift = 0; // For dynamic gradient effect across the pods

    // Apply dynamic gradient to the back three pods using the West Virginia palette
    for (int podIndex = 3; podIndex < 6; ++podIndex) { // Ensure iteration over back three pods
        for (int ledIndex = 0; ledIndex < NUM_LEDS_PER_POD; ++ledIndex) {
            uint8_t colorIndex = (paletteShift + ledIndex * 256 / NUM_LEDS_PER_POD) % 256;
            pods[podIndex].leds[ledIndex] = ColorFromPalette(westVirginiaPalette, colorIndex, 255, LINEARBLEND);
        }
    }

    // Handling for middle pods based on the isLeft flag, ensuring only one is highlighted
    int gradientPodIndex = isLeft ? 1 : 2; // Determine which middle pod gets the gradient
    for (int ledIndex = 0; ledIndex < NUM_LEDS_PER_POD; ++ledIndex) {
        uint8_t colorIndex = (paletteShift + ledIndex * 256 / NUM_LEDS_PER_POD) % 256;
        pods[gradientPodIndex].leds[ledIndex] = ColorFromPalette(westVirginiaPalette, colorIndex, 255, LINEARBLEND);
    }

    // Set the non-active middle pod to black (turned off)
    int nonActivePodIndex = isLeft ? 2 : 1; // The opposite middle pod
    fill_solid(pods[nonActivePodIndex].leds, NUM_LEDS_PER_POD, CRGB::Black);
    paletteShift += 1; // Increment for gradient movement

}

void handleRedLightCommand() {
    // First, turn off all LEDs for all pods except the ones involved in RedLight
    for (int i = 0; i < NUM_PODS; ++i) {
        if (i != 0 && i != 1 && i != 2 && i != 4) { // Exclude front pod, pod 1, pod 2, and back middle pod
            fill_solid(pods[i].leds, NUM_LEDS_PER_POD, CRGB::Black);
                pods[1].leds[2] = CRGB::Black; // 3rd LED
                pods[1].leds[3] = CRGB::Black; // 4th LED

                // Set pod 2's 1st and 2nd LED on
                pods[2].leds[0] = CRGB::Black; // 1st LED
                pods[2].leds[1] = CRGB::Black; // 2nd LED
        }

    }

    // Turn the front pod (pod 0) green
    fill_solid(pods[0].leds, NUM_LEDS_PER_POD, CRGB::Green);

    pods[1].leds[0] = CRGB::Yellow; // 3rd LED
    pods[1].leds[1] = CRGB::Yellow; // 4th LED

    // Set pod 2's 1st and 2nd LED on
    pods[2].leds[2] = CRGB::Yellow; // 1st LED
    pods[2].leds[3] = CRGB::Yellow; // 2nd LED

    fill_solid(pods[4].leds, NUM_LEDS_PER_POD, CRGB::Red);

}

void handleLastSensor() {
    turnOffLEDs();
    // Increment the rainbow hue over time for a continuous effect
    rainbowHue += 5; 
    
    pods[1].leds[0] = CHSV(rainbowHue, 255, 255); // 3rd LED in pod 1
    pods[1].leds[1] = CHSV(rainbowHue, 255, 255); // 4th LED in pod 1

    pods[2].leds[2] = CHSV(rainbowHue, 255, 255); // 1st LED in pod 2
    pods[2].leds[3] = CHSV(rainbowHue, 255, 255); // 2nd LED in pod 2

}



void updateLEDsFromCommand(String command) {
    command.trim();
    if (command == "-1") {
        turnOffLEDs();
        FastLED.setBrightness(200);
    } else if (command == "startup") {
        runStartupAnimation();
        return;
    } else if (command == "beerball"){
        runBeerBallStartUp();
        return;
    }
    else if (command == "lvc"){
        lvc();
        return;
    }
    else if (command == "LS") {
        handleLastSensor();
    } 
    else if (command == "BBR"){
        handleBeerBall();
    }
    else if(command == "Winner"){
      updateAnimation("Winner");
    }
    else if(command == "Loser"){
      updateAnimation("Loser");
    }
    else {
        processLEDCommand(command);
    }


    safeFastLEDShow();
}

void handleBeerBall(){
   fill_solid(pods[4].leds, NUM_LEDS_PER_POD, CRGB::Red);
}

void turnOffLEDs() {
  for (int i = 0; i < NUM_PODS; ++i) {
    fill_solid(pods[i].leds, NUM_LEDS_PER_POD, CRGB::Black);
  }
}

void safeFastLEDShow() {
    try {
        FastLED.show();
    } catch (const std::exception& e) {
        // Handle standard exceptions
        Serial.println("Caught an exception from FastLED.show()");
        Serial.println(e.what());
        // Optionally, reset the device or take corrective action
        ESP.restart();
    } catch (...) {
        // Catch all other exceptions
        Serial.println("Caught an unspecified exception from FastLED.show()");
        ESP.restart();
    }

}



void processLEDCommand(String command) {
    if (command.length() == 0) {
        return; 
    }

    int podIndex = 0;
    CRGB color = CRGB::Black; // Default color
    bool irSignalDetected = command.indexOf("IR") != -1;

  for (unsigned int i = 0; i < command.length(); ++i) {
        if (command[i] == 'P' && i + 1 < command.length()) {
            podIndex = command[i + 1] - '0' - 1;
            if (podIndex < 0 || podIndex >= NUM_PODS) {
                Serial.println("Invalid pod index.");
                continue; // Skip if pod index is invalid
            }

            int colorIndex = command.indexOf(':', i) + 1;
            if (colorIndex <= 0 || colorIndex + NUM_LEDS_PER_POD > command.length()) {
                Serial.println("Malformed command: incomplete data for pod.");
                break; // Exit the loop if data is incomplete
            }

            char colorCode = command[colorIndex];
            switch (colorCode) {
                case 'L': color = CHSV(rainbowHue, 255, 255); rainbowHue = (rainbowHue + 10) % 256; break;
                case 'G': color = CRGB::Green; break;
                case 'B': color = CRGB::Blue; break;
                case 'T': handleTCupPattern(); return;
                case 'R': handleRedLightCommand(); return;
                case 'W':
                    if (command.substring(colorIndex, colorIndex + 2) == "WL") {
                        handleWestVirginiaCommand(command, true); // True for left side
                        continue;
                    } else if (command.substring(colorIndex, colorIndex + 2) == "WR") {
                        handleWestVirginiaCommand(command, false); 
                        continue;
                    }
                    continue;
               default: 
                    Serial.println("Unknown color code.");
                    color = CRGB::Black; 
                    break;
            }

              for (int ledIndex = 0; ledIndex < NUM_LEDS_PER_POD; ledIndex++) {
                if (colorIndex + 1 + ledIndex < command.length()) {
                    bool isOn = command[colorIndex + 1 + ledIndex] == '1';
                    pods[podIndex].leds[ledIndex] = isOn ? color : CRGB::Black;
                }
            }
            if (irSignalDetected) {
              // Specific LEDs turn blue for the middle sensor
              pods[1].leds[0] = CRGB::Blue;
              pods[1].leds[1] = CRGB::Blue;
              pods[2].leds[2] = CRGB::Blue;
              pods[2].leds[3] = CRGB::Blue;
          }

      }
  }

}

void ledControlTask(void *pvParameters) {
    for (;;) {
        processSerialCommands();
        esp_task_wdt_reset(); 
        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

void processSerialCommands() {
    static char commandBuffer[COMMAND_BUFFER_SIZE];
    static int commandBufferIndex = 0;

    while (MySerial2.available()) {
        char c = MySerial2.read();
        if (c == '\n') {
            commandBuffer[commandBufferIndex] = '\0'; // Terminate the string
            String command(commandBuffer);
            // Reset the buffer index for the next command
            commandBufferIndex = 0;
            updateLEDsFromCommand(command);
        } else if (commandBufferIndex < COMMAND_BUFFER_SIZE - 1) {
            commandBuffer[commandBufferIndex++] = c;
        }
    }
}


void sensorTask(void * pvParameters) {
    
    esp_task_wdt_add(NULL);
    int irSensorValues[IR_FILTER_WINDOW_SIZE] = {0}; 
    int irReadIndex = 0; // Index for the next reading to store
    int irSensorValueFiltered = 0; // Variable to store the filtered value
    char sensorData[SENSOR_DATA_BUFFER_SIZE];

    for (;;) {
        int dataLength = 0;
        for (int i = 0; i < NUM_PODS; ++i) {
            int sensorValue = analogRead(pods[i].sensorPin);
            if(i == 2){
              sensorValue = sensorValue - 5;
            }
            dataLength += snprintf(sensorData + dataLength, SENSOR_DATA_BUFFER_SIZE - dataLength, "%d,", sensorValue);
        }

        // Read and filter the IR sensor value
        irSensorValues[irReadIndex] = analogRead(IR_SENSOR_PIN);
        irReadIndex = (irReadIndex + 1) % IR_FILTER_WINDOW_SIZE; // Move to the next index, wrapping around if necessary
        
        // Calculate the moving average
        int sum = 0;
        for (int i = 0; i < IR_FILTER_WINDOW_SIZE; ++i) {
            sum += irSensorValues[i];
        }
        irSensorValueFiltered = sum / IR_FILTER_WINDOW_SIZE;

        // Append the filtered IR sensor value with a distinct identifier
        snprintf(sensorData + dataLength, SENSOR_DATA_BUFFER_SIZE - dataLength, "IR:%d", irSensorValueFiltered);
        if(!isStartupAnimationRunning){
          MySerial2.print('R');
          MySerial2.println(sensorData);
   
        }

        esp_task_wdt_reset();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void runStartupAnimation() {
    const int animationDuration = 15000; // Total duration of the animation in milliseconds
    const int pulseDuration = 3000; // Duration of one pulse cycle (fade in and out) in milliseconds
    const CRGB targetColor = CRGB::Blue; // Target color for the fade-in

    unsigned long startTime = millis();
    while (millis() - startTime < animationDuration) {
        unsigned long currentTime = millis() - startTime;
        int pulsePhase = currentTime % pulseDuration;
        float fadeAmount = (pulsePhase < pulseDuration / 2) ? (float)pulsePhase / (pulseDuration / 2) : 1.0f - (float)(pulsePhase - pulseDuration / 2) / (pulseDuration / 2);

        CRGB fadedColor = targetColor;
        fadedColor.fadeToBlackBy(255 * (1 - fadeAmount)); 

        // Apply faded color to all LEDs
        for (int podIndex = 0; podIndex < NUM_PODS; ++podIndex) {
            fill_solid(pods[podIndex].leds, NUM_LEDS_PER_POD, fadedColor);
        }
        // Control the update rate for a smoother animation
        esp_task_wdt_reset(); // Keep the watchdog happy

       safeFastLEDShow();
    }
}

void lvc() {
  const int animationDuration = 30000;  // Total duration of the animation in milliseconds
  const int pulseDuration = 10000;       // Duration of one pulse cycle (fade in and out) in milliseconds
  const CRGB targetColor = CRGB::Blue;  // Target color for the fade-in

  unsigned long startTime = millis();
  while (millis() - startTime < animationDuration) {
    unsigned long currentTime = millis() - startTime;
    int pulsePhase = currentTime % pulseDuration;
    float fadeAmount = (pulsePhase < pulseDuration / 2) ? (float)pulsePhase / (pulseDuration / 2) : 1.0f - (float)(pulsePhase - pulseDuration / 2) / (pulseDuration / 2);

    CRGB fadedColor = targetColor;
    fadedColor.fadeToBlackBy(255 * (1 - fadeAmount)); 

    // Apply faded color to all LEDs
    for (int podIndex = 0; podIndex < NUM_PODS; ++podIndex) {
      fill_solid(pods[podIndex].leds, NUM_LEDS_PER_POD, fadedColor);
    }
    // Control the update rate for a smoother animation
    esp_task_wdt_reset();  // Keep the watchdog happy :)

    safeFastLEDShow();
  }
}


void updateAnimation(const String& command) {
    CRGB color;
    // Update color based on the command
    if (command == "Winner") {
        color = CRGB::Green;
    } else if (command == "Loser") {
        color = CRGB::Red;
    }

    if (increasing) {
        brightness += 5; 
        if (brightness >= 255) { 
            brightness = 255;
            increasing = false; 
        }
    } else {
        brightness -= 5; 
        if (brightness <= 0) { 
            brightness = 0;
            increasing = true; 
        }
    }

    for (int podIndex = 0; podIndex < NUM_PODS; ++podIndex) {
        fill_solid(pods[podIndex].leds, NUM_LEDS_PER_POD, color);
    }
    FastLED.setBrightness(brightness);
}


void runBeerBallStartUp(){
  const unsigned long pulseDuration = 5000; 
  const unsigned long startTime = millis();
  const float twoPi = 6.28318530718; 
  const int pulseCycleDuration = 1000; 

  while (millis() - startTime < pulseDuration) {
      unsigned long elapsed = millis() - startTime;
      float phase = (float)(elapsed % pulseCycleDuration) / pulseCycleDuration;
      float sineValue = (sin(phase * twoPi) + 1.0) / 2.0; 
      uint8_t brightness = 255 * sineValue; 
      CRGB pulsingBlue = CHSV(HUE_BLUE, 255, brightness);
      fill_solid(pods[4].leds, NUM_LEDS_PER_POD, pulsingBlue);
      safeFastLEDShow();
      delay(10); 
  }
 
  fill_solid(pods[4].leds, NUM_LEDS_PER_POD, CRGB::Black);
  safeFastLEDShow();
}
void setup() {
  Serial.begin(9600);
  MySerial2.begin(19200, SERIAL_8N1, RXp2, TXp2);
  FastLED.setBrightness(200);
  WiFi.mode(WIFI_OFF);
  btStop();
  esp_bt_controller_disable();

  setupPods();
  pinMode(IR_SENSOR_PIN, INPUT);

  // Create tasks for sensor reading and LED control, ensuring they don't block each other
  xTaskCreate(sensorTask, "SensorTask", 4096, NULL, 1, NULL);
  xTaskCreatePinnedToCore(ledControlTask, "LEDControlTask", 4096, NULL, 1, NULL, 1);

  // Configure and enable the watchdog timer
  esp_task_wdt_init(5, true); // Set timeout for 5 seconds
  esp_task_wdt_add(NULL); // Add the main loop task to WDT
}

void loop() {
  // Reset the watchdog timer in the main loop to prevent resets
  esp_task_wdt_reset();
  vTaskDelay(pdMS_TO_TICKS(1000)); // Yield to other tasks
}