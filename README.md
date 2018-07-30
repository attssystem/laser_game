# Laser InfraRed Game (aka LIR Game)

## Documentation

### Based on these technologies :
- Core
  - Arduino Nano
  - Tailor-made PCB
- Communications
  - nRF24l01+ (2,4GHz)
  - YIRTM IR Transmitter
- Interface
  - SD1306 128x64
  - Piezo Buzzer
  - Laser (+220Ohm Resistor)
  - Button (using internal pullup)
- Power
  - StepUp 3.3v to 5v
  - 3000mAh 3.7v Battery
  - MicroUSB battery circuit charger
  - Key power switch

### Wiring
```
                        TX 31       |             |   VIN
                        RX 30       |             |   GND --- GND IR & nRF24 & Screen
                        RST 29      |             |   RST 29
                        GND         |             |   5V --- VCC IR + Laser (220 Ohm)
              Rx IR --- D2 32       |             |   A7 22
              Tx IR --- D3 1        |             |   A6 19
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
- Software
  - [x] 'Wifi' working
  - [x] IR working
  - [x] Screen working
  - Configuration
    - [ ] Can change different parameters
    - [ ] Can change default parameters
    - [ ] Can change ID
    - [ ] Can change nRF24l01 channel
  - in Game
    - [x] Aim and shot
    - [x] Get shot
    - [x] Manage timed games
  - Ending
    - [x] Can calculate positions
    - [x] Can send positions to all weapons
    - [x] Bonus : Zelda Music
- Hardware
  - Electronics
    - [x] Autonomous power (battery powered)
    - [x] Charging circuit
    - [x] Secured startup
  - Design
    - [x] Enclosure design
    - [x] Printed design
