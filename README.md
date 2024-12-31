# Smart Pong Table with Interactive LED Lighting

Welcome to the **Ultimate Smart Pong Table** project! This is not your ordinary beer pong table—it's an interactive, fully automated, LED-illuminated, game-changing experience. Whether you're a casual player or a beer pong champion, this table brings a whole new level of excitement to the game.

![Smart Pong Table](https://github.com/user-attachments/assets/0a162419-b63b-4cd4-ad0c-d093686eba85)

## Table of Contents

- [Features](#features)
- [Hardware Overview](#hardware-overview)
- [Software Architecture](#software-architecture)
- [Setup and Installation](#setup-and-installation)
- [Game Modes](#game-modes)
- [How It Works](#how-it-works)
- [Gallery](#gallery)
- [Table Blueprints](#table-blueprints)
- [License](#license)

## Features

- **Dynamic LED Animations**: Watch as the table comes to life with vibrant LED patterns that react to game events.
- **Multiple Game Modes**: Choose from classic Beer Pong, Beer Ball, or the thrilling Sled Bob mode.
- **Interactive Gameplay**: IR sensors detect cup placement and ball interactions, updating scores in real time.
- **Central Scoreboard**: An LED matrix at the center displays scores, animations, and game state information.
- **Responsive Gameplay**: Dual-core ESP32 microcontrollers ensure smooth and responsive operation.
- **Customizable**: Open-source code allows you to tweak game rules, LED patterns, and more.

## Hardware Overview

The table is powered by an array of ESP32 microcontrollers and sensors, meticulously arranged to create an immersive gaming experience.

### Microcontrollers

- **Controller ESP32-S3-WROOM-1**: The brain of the operation, located beneath the central LED matrix. It controls the main game logic and the LED matrix display.
- **Peripheral ESP-WROOM-32 Units (x2)**: Located under each cup set (left and right), these handle cup detection and LED control for their respective sides.

### Sensors and Components

- **TCRT5000 IR Sensors**: Placed under each cup position to detect the presence of cups and interactions with the ball.
- **WS2812B Addressable LEDs**: Used for the cup LEDs and the central LED matrix, providing vibrant and customizable lighting.
- **Acrylic Top Panel**: Ensures smooth gameplay and optimal sensor performance by reducing IR noise.

## Software Architecture

The software is organized into a set of classes, each handling specific aspects of the game's functionality. Here's a rundown of the main components:

### Controller Software

- **Table Class**: Orchestrates the game states, processes sensor data, and delegates tasks to the cup sets and LED matrix.
- **CupSet Class**: Manages a set of cups on one side of the table, including their states and interactions.
- **Cup Class**: Represents an individual cup, handling presence detection and special states like "Island" or "Last Cup".
- **GameMode Class**: Provides a framework for different game modes, using a virtual function pattern to allow easy addition of new modes.
- **LedMatrix Class**: Manages the central LED matrix, displaying scores, animations, and game states.

### Peripheral Software

- **Core 1**: Dedicated to reading and filtering IR sensor data, transmitting it to the controller for processing.
- **Core 2**: Handles incoming LED commands from the controller and updates the cup LEDs accordingly.

## Setup and Installation

### Prerequisites

- **Arduino IDE**: Ensure you have the latest version installed.
- **ESP32 Board Support**: Install via the Arduino Boards Manager.
- **Required Libraries**:
  - FastLED
  - FastLED_NeoMatrix
  - Adafruit_GFX
  - HardwareSerial

### Steps

1. **Clone the Repository**:

   ```bash
   git clone https://github.com/yourusername/ultimate-beer-pong-table.git
   ```

2. **Hardware Assembly**: Follow the hardware schematics in the `docs` folder to assemble the table.
3. **Configure the Software**:
   - Open the Arduino IDE and load the projects for each microcontroller (`ControllerESP`, `LCupSet`, `RCupSet`).
   - Adjust the `Config.h` files if necessary to match your hardware setup.
4. **Upload the Code**:
   - Select the appropriate board and port in the Arduino IDE.
   - Upload the code to each ESP32 module.
5. **Test the Setup**: Power on the table and verify that the sensors and LEDs are functioning correctly.

## Game Modes

### Beer Pong

Enjoy the classic game with enhanced features:

- **Island Detection**: Automatically detects and highlights island cups.
- **Last Cup Animation**: Special animations play when a team is down to their last cup.
- **Score Tracking**: The central LED matrix displays the current score and cup counts.

### Beer Ball

A fast-paced variation with unique rules:

- **Reactive Side Cups**: Side cups light up based on player actions.
- **Speed-Based Animations**: LED patterns react to the pace of the game.

### Sled Bob

A custom game mode for added fun:

- **Dynamic Team Challenges**: Special conditions that teams must meet, displayed on the LED matrix.
- **Winning Celebrations**: Unique animations play for the winning team.

## How It Works

- **Cup Detection**: The TCRT5000 IR sensors detect the presence of cups by measuring reflected IR light through the acrylic top.
- **Data Processing**: Sensor data is transmitted from the peripheral ESP32s to the controller ESP32 via `HardwareSerial` communication.
- **Game Logic**: The controller processes the data, updates game states, and sends LED commands back to the peripherals.
- **Visual Feedback**: LEDs under each cup and the central LED matrix provide real-time visual feedback, enhancing gameplay.

## Gallery

Check out the table in action:

[**Watch the Video**](https://www.veed.io/view/157bbee4-b8ff-4abd-b34b-512a8b7193fd?panel=share)

## Table Blueprints

[**Download Table Blueprints**](https://github.com/user-attachments/files/18283329/Table.Blue.Prints.pdf)

## License

This project is licensed under the [MIT License](LICENSE).

---

**Cheers to endless fun and unforgettable beer pong nights!**

---

Feel free to dive into the code and customize the experience. Contributions are welcome!
