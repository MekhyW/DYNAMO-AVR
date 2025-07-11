# DYNAMO-AVR

Low-level C++ code created for the DYNAMO project

Runs on the Lattepanda Delta´s Arduino Leonardo co-processor

## Compilation Instructions

### Real Hardware

- Normal operation: Use standard Arduino IDE compilation

- Test mode on Arduino:
   1) Edit TestConfig.h and uncomment #define TEST_MODE
   2) Then compile with Arduino IDE

### PC Simulation

- Set config:
   1) Edit TestConfig.h and uncomment #define SIMULATE_HARDWARE
   2) If you want test mode, also uncomment #define TEST_MODE

- Compile with:
   ```bash
   cd AVR_software
   g++ -std=c++11 AVR_software.cpp -o AVR_software_sim -pthread
   ```

- Run the simulation:
   ```bash
   ./AVR_software_sim
   ```

## Using the CLI Interface

Once running, the CLI interface provides the following commands:

- `help` - Show available commands
- `leds <effect> <color_r> <color_g> <color_b> <brightness>` - Test LED effects
- `servo <index> <position>` - Move a specific servo
- `emotion <angry> <disgusted> <happy> <neutral> <sad> <surprised>` - Test emotion blending
- `inputs <14 comma-separated values>` - Set all inputs at once

Example usage:
```
> leds 0 148 0 211 50
LED test command sent

> servo 0 120
Servo 0 moved to position 120

> emotion 100 0 0 0 0 0
Emotion test command sent

> inputs 1,1,50,148,0,211,0,0,0,0,1,0,0,0
Inputs updated successfully
```