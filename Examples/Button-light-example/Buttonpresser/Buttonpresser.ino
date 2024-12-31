// Define button pins
const int button1 = 13;

// Define LED pins
const int led1 = 14;  // This one

void setup() {
  // Initialize serial communication at 115200 baud
  Serial.begin(115200);

  // Initialize button pins as input with pull-up
  pinMode(button1, INPUT_PULLUP);

  // Initialize LED pins as output
  pinMode(led1, OUTPUT);
}

void loop() {
  // Check the button and control the corresponding LED
  int buttonState = digitalRead(button1);
  digitalWrite(led1, !buttonState);  // Button1 controls LED1  
  delay(10);  // Short delay to reduce button bouncing effect
}