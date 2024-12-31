#include "CupSet.h"
#include "LedMatrix.h"
#include "FrameData.h"
#include <Config.h>


LedMatrix::LedMatrix() {
    leds = new CRGB[NUM_LEDS];
    matrix = new FastLED_NeoMatrix(leds, MATRIX_WIDTH, MATRIX_HEIGHT,
                                   NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
                                   NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);

                                
}

void LedMatrix::setupMatrix() {
    FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUM_LEDS);
    matrix->begin();
    matrix->setTextWrap(false);
    matrix->setBrightness(40);
}

void LedMatrix::clearDisplay() {
    for (int x = 0; x < MATRIX_WIDTH; x++) {
        for (int y = 0; y < MATRIX_HEIGHT; y++) {
            matrix->drawPixel(x, y, (uint32_t)0x000000); // Use the numeric color code for black
        }
    }
    matrix->show(); // Update the display with the new state
}

 void LedMatrix::updateBeerBallDisplay(){
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
      matrix->drawPixel(MATRIX_WIDTH / 2, y, (uint32_t)0xFFD700);
    }
 }

void LedMatrix::updateDisplay(CupSet& leftSet, CupSet& rightSet) {
    updateGlobalFadePhase(); 
    matrix->fillScreen(0);
    displayBorders(leftSet, rightSet);
    displaySpecialStates(leftSet, rightSet);
    displayScoreboard(leftSet, rightSet);
    updateColors(); 
    matrix->show();
}

void LedMatrix::displayBorders(CupSet& leftSet, CupSet& rightSet) {
    updateGlobalFadePhase(); 

    for (int x = 0; x < MATRIX_WIDTH; ++x) {
        for (int y = 0; y < MATRIX_HEIGHT; ++y) {
            bool isBorder = x == 0 || y == 0 || x == MATRIX_WIDTH / 2 - 1 || x == MATRIX_WIDTH / 2 || x == MATRIX_WIDTH - 1 || y == MATRIX_HEIGHT - 1;
            if (isBorder) {
                CRGB color = determineBorderColor(x, y, leftSet, rightSet);
                matrix->drawPixel(x, y, color);
            }
        }
    }
}

CRGB LedMatrix::determineBorderColor(int x, int y, CupSet& leftSet, CupSet& rightSet) {
    bool isLeftBox = x < MATRIX_WIDTH / 2;
    BeerPongState state = isLeftBox ? leftSet.getSpecialState() : rightSet.getSpecialState();
    bool alternatePixel = (x + y + int(globalFadePhase * 2)) % 2 == 0; 

    switch (state) {
        case CS_WestVirginia:
            return alternatePixel ? blend(CRGB::Yellow, CRGB::Blue, globalFadePhase * 255) : blend(CRGB::Blue, CRGB::Yellow, globalFadePhase * 255);
          // More cases to be added
        default:
            return isLeftBox ? leftCurrentColor : rightCurrentColor;
    }
}



void LedMatrix::displaySpecialStates(CupSet& leftSet, CupSet& rightSet) {
    updateTargetColors(leftSet, rightSet);
    CRGB leftColor = leftTargetColor;
    CRGB rightColor = rightTargetColor;
    CRGB middleColor = blend(leftColor, rightColor, 128);


}

void LedMatrix::updateTargetColors(CupSet& leftSet, CupSet& rightSet) {
    leftTargetColor = getColorForState(leftSet.getSpecialState());
    rightTargetColor = getColorForState(rightSet.getSpecialState());
}

CRGB LedMatrix::getColorForState(BeerPongState state) {
    switch (state) {
        case LastCup:
            return CHSV(hueValue++, 255, 255);
        case CS_Bermuda:
            return CRGB::Red;
        case CS_WestVirginia:
            return CRGB::Yellow;
        case CS_RedLight:
            return CRGB::Red;
        case CS_Island:
            return CRGB::Green;
        default:
            return CRGB::Blue;
    }
}

void LedMatrix::displayScoreboard(CupSet& leftSet, CupSet& rightSet) {
    int leftCount = leftSet.countCupsPresent();
    int rightCount = rightSet.countCupsPresent();
    char displayText[20];
    sprintf(displayText, "%d %d", leftCount, rightCount);
    int16_t x1, y1;
    uint16_t w, h;

    matrix->setTextColor(matrix->Color(255, 255, 255));
    matrix->getTextBounds(displayText, 0, 0, &x1, &y1, &w, &h);

    int16_t x = (MATRIX_WIDTH - w) / 2;
    int16_t y = (MATRIX_HEIGHT - h) / 2 + 1; 

    matrix->setCursor(x, y);
    matrix->print(displayText);
}


void LedMatrix::updateColors() {
    fadeColorsTowardsTargets();
}

void LedMatrix::fadeColorsTowardsTargets() {
    // Check if we need to update the left color
    if (leftCurrentColor != leftTargetColor) {
        leftCurrentColor = blend(leftCurrentColor, leftTargetColor, 10); 
    }
    // Check if we need to update the right color
    if (rightCurrentColor != rightTargetColor) {
        rightCurrentColor = blend(rightCurrentColor, rightTargetColor, 10); 
    }
}

void LedMatrix::updateGlobalFadePhase() {
    unsigned long currentTime = millis();
    const unsigned long cycleDuration = 3000; // Full cycle duration in milliseconds
    unsigned long cycleTime = currentTime % cycleDuration;
    globalFadePhase = (float)cycleTime / cycleDuration;
}

void LedMatrix::pongStartUp() {
    int NUM_FRAMES = 35;
    int frameSize = FRAME_WIDTH * FRAME_HEIGHT; 

    for(int currentFrame = 0; currentFrame < NUM_FRAMES; ++currentFrame) {
        int startIndex = currentFrame * FRAME_WIDTH; // Calculate start index of the current frame within the large array

        for (int y = 0; y < FRAME_HEIGHT; ++y) {
            for (int x = 0; x < FRAME_WIDTH; ++x) {
                int index = startIndex + (y * FRAME_WIDTH * NUM_FRAMES) + x; // Calculate the correct index within the large array
                uint32_t colorValue = pgm_read_dword_near(&(BeerPongStartUp[index])); 

            
                uint8_t red = (colorValue >> 24) & 0xFF;
                uint8_t green = (colorValue >> 16) & 0xFF;
                uint8_t blue = (colorValue >> 8) & 0xFF;

                // Update the matrix pixel
                matrix->drawPixel(x, y, CRGB(red, green, blue));
            }
        }

        FastLED.show(); 
        delay(100); 
    }
}

void LedMatrix::beerBallStartUp() {
    int NUM_FRAMES = 122;
    int frameSize = FRAME_WIDTH * FRAME_HEIGHT; 

    for(int currentFrame = 0; currentFrame < NUM_FRAMES; ++currentFrame) {
        int startIndex = currentFrame * FRAME_WIDTH; // Calculate start index of the current frame within the large array

        for (int y = 0; y < FRAME_HEIGHT; ++y) {
            for (int x = 0; x < FRAME_WIDTH; ++x) {
                int index = startIndex + (y * FRAME_WIDTH * NUM_FRAMES) + x; // Calculate the correct index within the large array
                uint32_t colorValue = pgm_read_dword_near(&(BeerBallStartUp[index])); 
                uint8_t red = (colorValue >> 24) & 0xFF;
                uint8_t green = (colorValue >> 16) & 0xFF;
                uint8_t blue = (colorValue >> 8) & 0xFF;

                matrix->drawPixel(x, y, CRGB(red, green, blue));
            }
        }
        FastLED.show(); 
        delay(100); 
    }
}

void LedMatrix::sledBobStartUp() {
    int NUM_FRAMES = 80;
    int frameSize = FRAME_WIDTH * FRAME_HEIGHT; 

    for(int currentFrame = 0; currentFrame < NUM_FRAMES; ++currentFrame) {
        int startIndex = currentFrame * FRAME_WIDTH; // Calculate start index of the current frame within the large array

        for (int y = 0; y < FRAME_HEIGHT; ++y) {
            for (int x = 0; x < FRAME_WIDTH; ++x) {
                int index = startIndex + (y * FRAME_WIDTH * NUM_FRAMES) + x; // Calculate the correct index within the large array
                uint32_t colorValue = pgm_read_dword_near(&(sledbob[index])); 
                uint8_t red = (colorValue >> 24) & 0xFF;
                uint8_t green = (colorValue >> 16) & 0xFF;
                uint8_t blue = (colorValue >> 8) & 0xFF;

                matrix->drawPixel(x, y, CRGB(red, green, blue));
            }
        }
        FastLED.show(); 
        delay(100); 
    }
}





void LedMatrix::displayFromArray() {
   
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
        for (int x = 0; x < MATRIX_WIDTH; x++) {
            uint32_t colorValue = pgm_read_dword_near(&lvc[x + y * MATRIX_WIDTH]);  
            uint8_t red = (colorValue >> 24) & 0xFF;
            uint8_t green = (colorValue >> 16) & 0xFF;
            uint8_t blue = (colorValue >> 8) & 0xFF;
            matrix->drawPixel(x, y, CRGB(red, green, blue));
        }
    }
    FastLED.show(); 
}





