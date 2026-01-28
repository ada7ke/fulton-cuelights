# Fulton Cue Lights

Wireless RGB LED controller system using ESP32-C3 microcontrollers with RF serial communication.

## Overview

Two-device system: a handheld controller with three buttons and a receiver that controls RGB LEDs. Designed for theater/stage cue lighting applications.

## Hardware

- **Controller**: ESP32-C3 with 3 push buttons (red, blue, green) + RGB LED feedback
- **Receiver**: ESP32-C3 with 3 PWM-controlled LEDs (red, blue, green)
- **Communication**: RF serial link (433MHz/915MHz modules)

### Pin Configuration

**Controller:**
- Buttons: Red=10, Blue=3, Green=4
- RGB LED: Red=2, Green=1, Blue=0
- Activity LED: 8

**Receiver:**
- LEDs: Red=6, Green=9, Blue=7
- Activity LED: 8

## Operating Modes

1. **Single Color** - One color at a time, auto-sleep after 5s
2. **Toggle** - Independent on/off for each color
3. **Hold** - LEDs active while buttons held

**Shortcuts:**
- Hold Red + Green: Cycle modes
- Hold Red + Blue: Cycle brightness (5, 15, 30)

**RGB LED Indicator**
RGB LED on controller indicates
1. LED color on/off
2. Yellow blinking - Mode/Brightness Index cycled to
3. Purple blinking - Error

## Setup

1. Install [PlatformIO](https://platformio.org/)
2. Copy `include/data.example.h` to `include/data.h` (edit if using WiFi/OTA features)
3. Build and upload to both devices
4. Ground pin 5 on controller device to set role

## Build

```bash
pio run -e esp32-c3-devkitm-1
pio run -e esp32-c3-devkitm-1 -t upload
```

## License

MIT
