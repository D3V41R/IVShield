# IVShield

IVShield is an automatic transfer switch and home power monitoring system. It detects power outages in under 30ms and seamlessly switches to a backup source — no human intervention required. Built from scratch including custom sensing circuitry, relay driver, and STM32 firmware with FreeRTOS.

## Features
- Automatic transfer to backup power within 30ms of dropout detection
- Hysteresis-based dropout detection at 85V RMS with 20V hysteresis to prevent false triggers
- Real-time OLED display showing line voltage, load voltage, RMS values, and active power source
- Zero crossing detection for fast and accurate dropout sensing
- Validated across 100 consecutive switching cycles with zero failures

## Hardware
- STM32F446RE Nucleo
- HF92F DPDT relay
- TLV7011 comparator
- MT3608 boost converter
- SSD1306 OLED display

<img width="4960" height="3507" alt="image" src="https://github.com/user-attachments/assets/52da0b6b-2e16-49e6-b782-83789a8e43d5" />
