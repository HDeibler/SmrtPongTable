#include <FastLED_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <FastLED.h>

#define PIN 17
#define MATRIX_WIDTH 22
#define MATRIX_HEIGHT 20
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

CRGB leds[NUM_LEDS];

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, MATRIX_WIDTH, MATRIX_HEIGHT,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);

int frame = 0; // Frame counter for animation
int obstacleX = MATRIX_WIDTH; // Start obstacle off-screen
bool isJumping = false;
int jumpHeight = 0;
int jumpPeak = 5; // Peak height of Mario's jump
int score = 0; // Score counter
int speed = 5; // Initial speed (delay in loop)
int speedIncreaseCounter = 0; // Counter to track when to increase speed

void setup() {
  FastLED.addLeds<WS2812B, PIN, GRB>(leds, NUM_LEDS);
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(40);
  Serial.begin(9600);
}

void resetGame() {
  score = 0; // Reset score
  speed = 5; // Reset speed
  obstacleX = MATRIX_WIDTH; // Reset obstacle position
}

void loop() {
  matrix->fillScreen(0); // Clear the matrix

  // Ground
  for (int x = 0; x < MATRIX_WIDTH; x++) {
    matrix->drawPixel(x, MATRIX_HEIGHT - 3, matrix->Color(0, 255, 0)); // Green ground
  }

  // Draw obstacle
  drawObstacle(obstacleX, MATRIX_HEIGHT - 5);

  // Mario
  int marioX = MATRIX_WIDTH / 4;
  int marioY = MATRIX_HEIGHT - 6 - jumpHeight; // Adjust Mario's Y position based on jump
  drawMario(marioX, marioY);

  // Handle jumping logic
  if (isJumping) {
    if (jumpHeight < jumpPeak) {
      jumpHeight++;
    } else {
      isJumping = false;
    }
  } else {
    if (jumpHeight > 0) {
      jumpHeight--;
    }
  }


  obstacleX -= 1;
  if (obstacleX < 0) { 
    obstacleX = MATRIX_WIDTH; 
    score++; 
    speedIncreaseCounter++;
  }

  
  if (speedIncreaseCounter >= 5) {
    speed = max(1, speed - 1);
    speedIncreaseCounter = 0; 
  }

  // Check for collision (miss)
  if (obstacleX == marioX && jumpHeight < 3) { 
    resetGame(); 
  }

  
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == ' ') { 
      isJumping = true;
      jumpHeight = 1; 
    }
  }

  // Display score
  matrix->setCursor(0, 0);
  matrix->setTextColor(matrix->Color(255, 255, 0));
  matrix->print(score);

  matrix->show();
  delay(speed * 20); 
}

void drawObstacle(int x, int y) {
  matrix->fillRect(x, y, 2, 2, matrix->Color(255, 0, 0)); 
}


void drawMario(int x, int y) {
  // Define Mario's colors
  uint32_t red = matrix->Color(255, 0, 0);
  uint32_t blue = matrix->Color(0, 0, 255);
  uint32_t skin = matrix->Color(255, 224, 189);
  uint32_t white = matrix->Color(255, 255, 255);

  // Hat
  matrix->drawPixel(x, y, red);
  matrix->drawPixel(x - 1, y, red);
  matrix->drawPixel(x + 1, y, red);

  // Face
  matrix->drawPixel(x, y + 1, skin);
  matrix->drawPixel(x - 1, y + 1, skin);
  matrix->drawPixel(x + 1, y + 1, skin);

  // Eyes
  matrix->drawPixel(x - 1, y + 1, white);
  matrix->drawPixel(x, y + 1, skin);

  // Body
  matrix->drawPixel(x, y + 2, blue);

  // Arms
  matrix->drawPixel(x - 1, y + 2, red);
  matrix->drawPixel(x + 1, y + 2, red);

  // Legs - Adjust based on the animation frame to simulate running
  if (frame == 0) {
    matrix->drawPixel(x - 1, y + 3, blue);
    matrix->drawPixel(x + 1, y + 3, blue);
  } else {
    matrix->drawPixel(x, y + 3, blue);
  }
}
