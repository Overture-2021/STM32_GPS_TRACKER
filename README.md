# STM32F103 GPS Logger (SIM900A + SD/FATFS)

Firmware for an STM32F103 application that captures GPS NMEA data, logs it to an SD card, and can forward the same data over GPRS via a SIM900A module. The top-level code lives in `USER/main.c` and is built with the STM32 Standard Peripheral Library.

## What it does
- Collects GPS NMEA frames (GPRMC/GNRMC) on USART1 via interrupt buffering.
- Builds a timestamped log filename by querying `time.nist.gov`.
- Appends raw GPS strings to a file on SD (FATFS over SPI1).
- Sends GPS strings over TCP using SIM900A GPRS.
- Supports manual and automatic send/log modes using onboard keys, with LED status.

## Operational modes
The application supports:
- **Manual mode**: send/log only when the WKUP key is pressed.
- **Automatic mode**: periodically send/log in the main loop (1s delay).
The mode of operation is reflected by the LED.

## Power-Up Sequence
- LED0 and LED1 are driven to HIGH
- Initializes core peripherals including UART, Delay, NVIC Pritority, etc.
- LED0 Blinks
- Detects GSM module establish connecting to time.nist.gov and logging server
- Initialize logging file in SD card
- LED1 Blinks
- LED0 is driven to LOW, LED1 is driven to HIGH

## Project layout
- `USER/` main application, startup, and project files (`OLED.uvprojx`).
- `SYSTEM/` core system utilities (delay, sys, USART1 GPS buffer).
- `HARDWARE/` peripheral drivers (SIM900A, SD/MMC, LED, KEY, SPI, USART2).
- `FATFS/` and `MALLOC/` FatFS integration and memory support.
- `STM32F10x_FWLib/` STM32 Standard Peripheral Library.
- `CORE/` CMSIS core support.

## Hardware pin mappings (from code)
- USART1 (GPS NMEA):
  - TX -> PA9
  - RX -> PA10
- USART2 (SIM900A):
  - TX -> PA2
  - RX -> PA3
- USART3 (debug/log output):
  - TX -> PB10
  - RX -> PB11
- SD card (SPI1):
  - CS -> PA4 (`SD_CS` in `HARDWARE/SD/mmc_sd.h`)
  - SCK -> PA5 (SPI1_SCK)
  - MISO -> PA6 (SPI1_MISO)
  - MOSI -> PA7 (SPI1_MOSI)
- LEDs:
  - LED0 -> PA8
  - LED1 -> PD2
- Keys:
  - KEY0 -> PC5
  - KEY1 -> PA15
  - WKUP -> PA0

## GPRS/TCP configuration
- GSM detection and TCP connect logic are in `HARDWARE/SIM900A/sim900a.c`.
- Server IP/port are currently set in `USER/main.c` (look for `SIM900A_CONNECT_SERVER_SEND_INFOR`).

## Build notes
This project is set up for Keil uVision (see `USER/OLED.uvprojx`).

Typical flow:
1. Open `USER/OLED.uvprojx` in Keil.
2. Build and flash to an STM32F103 target.
3. Connect GPS module to USART1, SIM900A to USART2, SD to SPI1.

## Files of interest
- `USER/main.c`: application entry point and mode control
- `SYSTEM/usart/usart.c`: GPS buffering (USART1 IRQ)
- `HARDWARE/SIM900A/sim900a.c`: SIM900A AT/GPRS helpers
- `HARDWARE/SD/MMC_SD.C`: SD card driver (SPI)
- `FATFS/`: FATFS integration

## Notes
- The GPS data is stored and transmitted as raw NMEA strings.
- Filename generation relies on the time server response format from `time.nist.gov`.
- The project uses libraries provided by ALIENTEK
