# Laser InfraRed Game (aka LIR Game)

## Description

  This Laser InfraRed Game is an Arduino-made project which consist of a 3D printed weapon (a bit looking like the Rick's portal gun) and a few basics components. This LIR Game is highly customisable and is really cheap. You can play with up to 10 players and you shot someone at 5 meters in daylight.

## Documentation

### Electronics :
- Core
  - Arduino Nano
  - Tailor-made PCB
- Communications
  - nRF24l01+ (2,4GHz)
  - YIRTM IR Transmitter
- Interface
  - SSD1306 128x64
  - Piezo Buzzer
  - Laser (+220Ohm Resistor)
  - Button (using internal pullup)
- Power
  - StepUp 3.3v to 5v
  - 3000mAh 3.7v Battery
  - MicroUSB battery circuit charger
  - Key power switch

### Arduino Libs
Available under LIR_prgm/lib
- Screen
  - Wire
  - ~~Adafruit_SSD1306~~
  - ~~Adafruit_GFX~~
  - SSD1306Ascii (lighter RAM usage)
- nRF24l01
  - SPI
  - nRF24l0
  - RF24
- Others
  - EEPROM
  - ~~ArduinoSort~~
- Thanks
  - ~~to emilv [ArduinoSort Git](https://github.com/emilv/ArduinoSort)~~
  - ~~to Adafruit [SSD1306 Git](https://github.com/adafruit/Adafruit_SSD1306) / [GFX Git](https://github.com/adafruit/Adafruit-GFX-Library)~~
  - to greiman [SSD1306Ascii Git](https://github.com/greiman/SSD1306Ascii)
  - to nRF24 [nRF24l01 and RF24 Git](https://github.com/nRF24/RF24)


### Wiring
```
              Rx IR --- TX 31       |             |   VIN
              Tx IR --- RX 30       |             |   GND --- GND IR & nRF24 & Screen
                        RST 29      |             |   RST 29
                        GND         |             |   5V --- VCC IR + Laser (220 Ohm)
                        D2 32       |             |   A7 22
                        D3 1        |             |   A6 19
           CE nRF24 --- D4 2        |   Arduino   |   SCL 28 --- SCL Screen
          CSN nRF24 --- D5 9        |    Nano     |   SDA 27 --- SDA Screen
              Piezo --- D6 10       |     V3      |   A3 26
                        D7 11       |             |   A2 25
                        D8 12       |     Old     |   A1 24
     Trigger button --- D9 13       |  Bootloader |   A0 23
                        D10 14      |             |   AREF 21
         MOSI nRF24 --- D11 15      |             |   3V3 --- VCC nRF24 & Screen
         MISO nRF24 --- D12 16      |             |   SCK 17 -- SCK nRF24
```

### PCB Schematics
Gerber files available in PCB folder

## Evolution
### Software
- [x] nRF24 working
- [x] IR working
- [x] Screen working
- Configuration
  - [x] Can change different inGame parameters
  - [x] Can change default parameters
  - [x] Can change ID
  - [x] Can change nRF24l01 channel
  - [x] Can send configuration data
- in Game
  - [x] Aim and shot
  - [x] Get shot
  - [x] Manage timed games
  - [x] Display timed
  - [x] Display score
- Ending
  - [x] Can calculate positions
  - [x] Can send positions to all weapons
  - [x] Bonus : Zelda Music
### Hardware
- Electronics
  - [x] Autonomous power (battery powered)
  - [x] Charging circuit
  - [x] Secured startup
- Design
  - [x] Enclosure design
  - [x] Printed design

## Known Issues
- ~~Can't use 128*64 pixels because lib takes too much memory and it looks like one of my screen get used to 32 pixels...~~
- Annoying screen clearing process
