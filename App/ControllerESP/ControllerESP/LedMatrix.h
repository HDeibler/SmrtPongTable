#ifndef LEDMATRIX_H
#define LEDMATRIX_H

class CupSet;
#include <Config.h>




class LedMatrix {
public:
    LedMatrix();
    void setupMatrix();
    void updateDisplay(CupSet& leftSet, CupSet& rightSet);
    void updateBeerBallDisplay();
    void clearDisplay();
    void pongStartUp();
    void beerBallStartUp();
    void displayFromArray();
    
private:
    CRGB* leds;
    FastLED_NeoMatrix* matrix;
    CRGB leftTargetColor;
    CRGB rightTargetColor;
    CRGB leftCurrentColor;
    CRGB rightCurrentColor;
    int hueValue = 0; 
    float globalFadePhase = 0.0;
    bool isBeerBallMode = false;




    void updateGlobalFadePhase();
    void displayBorders(CupSet& leftSet, CupSet& rightSet);
    void displaySpecialStates(CupSet& leftSet, CupSet& rightSet);
    void displayScoreboard(CupSet& leftSet, CupSet& rightSet);
    void updateColors();
    void fadeColorsTowardsTargets();
    void updateTargetColors(CupSet& leftSet, CupSet& rightSet);
    CRGB getColorForState(BeerPongState state);
    CRGB determineBorderColor(int x, int y, CupSet& leftSet, CupSet& rightSet);

};

#endif