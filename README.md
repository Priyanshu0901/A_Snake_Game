# AI Snake

A classic Snake game implementation running on an STM32F411RE microcontroller with AI

![STM32](https://img.shields.io/badge/STM32-F411RE-blue)
![Display](https://img.shields.io/badge/Display-WS2812B-green)
![IDE](https://img.shields.io/badge/IDE-STM32CubeIDE-orange)

## Overview

This project brings the classic Snake game to life on embedded hardware using an STM32 microcontroller. The game displays on a colorful WS2812B 8x8 LED matrix, creating a vibrant "Rainbow Snake" experience. Players can control the snake using a keyboard matrix, navigating to collect food and grow longer while avoiding collisions with walls and the snake's own body or Let the AI play it for you. 

## Demo Video (AI Autoplay Mode)

![](https://github.com/Priyanshu0901/A_Snake_Game/blob/main/assets/Snake_Autoplay.gif)

## Features

- **Classic Snake Gameplay**: Navigate the snake to collect food and grow longer
- **AI Autoplay Mode**: Watch the snake play itself using Hamiltonian cycle and path optimization algorithms
- **RGB LED Display**: Colorful WS2812B LED matrix (8x8) for vibrant visuals
- **Responsive Controls**: 8x8 keyboard matrix input for directional control
- **Embedded System**: Runs on STM32F411RE Nucleo board
- **Compact Design**: Fits within 64 LEDs (8x8 matrix)
- **I/O Expansion**: PCF8574 I/O expander support for additional features

## Hardware Requirements

| Component | Specification | Quantity |
|-----------|--------------|----------|
| **Microcontroller** | NUCLEO-F411RE (STM32F411RE) | 1 |
| **LED Display** | WS2812B 8x8 LED Matrix | 1 |
| **Input Device** | 8x8 Keyboard Matrix | 1 |
| **I/O Expander** | PCF8574 Module | 1 |
| **Power Supply** | 5V Power Supply (for LEDs) | 1 |
| **Miscellaneous** | Jumper wires, Breadboard | As needed |

### Pin Configuration

The exact pin configuration depends on your setup, but typically:
- **WS2812B Data Pin**: Connected to a GPIO pin (PB0 )
- **Keyboard Matrix**: Connected to PCF8574 via I2C1
- **PCF8574 **: Connected via I2C (SCL - PB7/SDA - PB6)

> **Note**: Check the `.ioc` file for the exact pin configuration used in this project.

## Software Requirements

- **IDE**: STM32CubeIDE
- **Programmer**: ST-LINK (built into Nucleo board)
- **Optional**: Serial terminal for debugging ->STLINK VCOM (115200 baud)

## Hardware Setup

### 1. WS2812B LED Matrix Connection

```
STM32F411RE    →    WS2812B Matrix
────────────────────────────────
PB0            →    DIN (Data Input)
5V (External)  →    VCC
GND            →    GND
```

> **Important**: WS2812B LEDs can draw significant current. Use an external 5V power supply for the LED matrix. The Nucleo board's 5V pin may not provide sufficient current.

### 2. Keyboard Matrix Connection

Connect the 8x8 keyboard matrix to GPIO pins:
- 8 pins for rows
- 8 pins for columns

Configure these pins in the `.ioc` file using STM32CubeMX.

### 3. PCF8574 I/O Expander

```
STM32F411RE    →    PCF8574
──────────────────────────
PB7 (SCL)      →    SCL
PB6 (SDA)      →    SDA
3.3V           →    VCC
GND            →    GND
```

## Installation

### Step 1: Clone the Repository

```bash
git clone https://github.com/Priyanshu0901/A_Snake_Game.git
cd A_Snake_Game
```

### Step 2: Open in STM32CubeIDE

1. Launch **STM32CubeIDE**
2. Go to `File` → `Open Projects from File System`
3. Browse to the cloned repository folder
4. Click `Finish`

### Step 3: Build the Project

1. Select the project in the Project Explorer
2. Click `Project` → `Build All` (or press `Ctrl+B`)
3. Wait for the build to complete successfully

### Step 4: Flash to Nucleo Board

1. Connect your NUCLEO-F411RE board via USB
2. Click `Run` → `Debug` (or press `F11`)
3. The program will be flashed to the microcontroller
4. Click `Resume` to start execution

### Step 5: Configure AI Mode (Optional)

To enable the AI autoplay feature:

1. Open `Core/Inc/main.h`
2. Add or uncomment the following line:
   ```c
   #define AI
   ```
3. Rebuild the project (`Ctrl+B`)
4. Flash to the board again

When AI mode is enabled, the snake will play automatically using a Hamiltonian cycle algorithm with path optimization for efficient gameplay.

## Game Controls

Use the keyboard matrix to control the snake:

| Key Direction | Action |
|---------------|--------|
| Up Arrow      | Move Up |
| Down Arrow    | Move Down |
| Left Arrow    | Move Left |
| Right Arrow   | Move Right |

> **Note**: The exact key mapping depends on your keyboard matrix layout. Check the code to customize key assignments.

## Technical Details

### WS2812B Control 
```c
WS2812B.h/.c
Display.h/.c
```

The WS2812B LEDs are controlled using PWM with DMA for precise timing. Each LED requires 24 bits of data (8 bits each for Red, Green, and Blue). The LEDs are daisy-chained, so data is shifted through all 64 LEDs sequentially.

**Timing Requirements:**
- **T0H**: 0.4µs ± 150ns (Logic 0, high time)
- **T0L**: 0.85µs ± 150ns (Logic 0, low time)
- **T1H**: 0.8µs ± 150ns (Logic 1, high time)
- **T1L**: 0.45µs ± 150ns (Logic 1, low time)
- **Reset**: >50µs low

### Game Logic 
```c
Game.h/.c
Input.h/.c
Canvas.h/.c
```

The game runs in a main loop that:
1. Reads keyboard input
2. Updates snake position
3. Checks for collisions (walls, self)
4. Checks for food collection
5. Updates LED matrix display
6. Adds delay for game speed control


## Troubleshooting

### LEDs Not Lighting Up

- **Check power supply**: Ensure the WS2812B matrix has adequate 5V power.
- **Verify data pin**: Confirm the correct GPIO pin is configured for PWM output.
- **Timing issues**: WS2812B requires precise timing. Check your timer configuration.

### Keyboard Not Responding

- **Check connections**: Verify all row and column pins are correctly connected.

### Game Runs Too Fast/Slow

Adjust tick rates in the Game.h to control game speed.

---

## Enjoy playing Rainbow Snake!

*If you found this project helpful, please give it a ⭐ star!*