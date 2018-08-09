/*****************************************************************************************************
  LaserInfraRed Game

  Hugues Kadi - ATTSSystem - All rights reserved ©

  Schematics
                                    _______________
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
                                    _______________

*****************************************************************************************************/

// nRF24 requirements

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(4, 5);
const byte addresses[][6] = {"00001", "00002", "00003", "00004", "00005", "00006", "00007", "00008", "00009", "00010"};

// Generic requirements

#include <EEPROM.h>

// Vars initialisation

byte data;
bool sendRight = false;
bool c = false;
bool c1 = false;
bool c2 = false;
bool c3 = false;
byte ID;
byte addR;
byte channel;
byte addT;
byte weaponNb;
byte gameTime;
byte scorePlus;
byte scoreMinus;
byte newMinutes;
byte newSeconds;
byte oldMinutes;
byte oldSeconds;
#define weaponNbDef 0
#define gameTimeDef 1
#define scorePlusDef 2
#define scoreMinusDef 3
#define IDDef 4
#define channelDef 5
#define trigger 7
#define speakerPin 9
byte receivedID;
long score = 0;
unsigned long timeStart;
unsigned long timeVal;
unsigned long timeRemain;
int delayTime;

// Screen requirements

#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>
#define I2C_ADDRESS 0x3C

SSD1306AsciiAvrI2c oled;

// Functions requirements

#include <functions.h>
#include <sound.h>
#include <configuration.h>

// Setup code

void setup() {

  // Reading ID and Channel in memory

  EEPROM.get(IDDef, ID);
  EEPROM.get(channelDef, channel);

  // Screen Configuration

  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.set2X();
  oled.setCursor(40, 0);
  oled.print(F("ATTS"));
  oled.setCursor(28, 2);
  oled.print(F("System"));
  oled.setCursor(18, 6);
  oled.set1X();
  oled.print(F("LIR Game (c) MK5"));
  delay(3000);
  oled.setCursor(0, 0);
  oled.set2X();
  oled.clear();
  oled.println(F("ID actuel :"));
  oled.print(ID);
  delay(1000);

  // Generic configuration (Inputs-Outputs)

  pinMode(trigger, INPUT_PULLUP );
  pinMode(speakerPin, OUTPUT);

  // Parameters configuration

  //configuration();

  // IR Rx-Tx configuration

  Serial.begin(9600);
}

// Loop code

void loop() {
  /*while (1==1){
    const byte dataBytes[5] PROGMEM = {0xA1, 0xF1, ID, 0xAA, 0xAA};
    Serial.write(dataBytes, sizeof(dataBytes));
    delay(500);
  }*/
  play();
  //returnStart();
  //ending();
}

void play() {
  timeStart = millis();

  // Playing during the game time

  while (timeVal < 60000*gameTime) {
    timeVal = millis() - timeStart;
    updateTime();

    // If bullet received

    if (Serial.available()) {
      receivedID = Serial.read();
      if (receivedID != ID && receivedID <= weaponNb+1) {
        data = 44;
        addT = receivedID - 1;
        radio.stopListening();
        radio.openWritingPipe(addresses[addT]);
        radio.write(&data, sizeof(data));
        delay(100);
        radio.startListening();
        score = score - scoreMinus;
        //draw(crossed, 500);
        updateDisplay();
      }
    }

    // If signal received from nRF24

    if (radio.available()) {
      radio.read(&data, sizeof(data));
      if (data == 44) {
        //draw(ticked, 500);
        score = score + scorePlus;
        updateDisplay();
      }
    }

    // If trigger is pushed

    if (digitalRead(trigger) == LOW) {
      const byte dataBytes[5] PROGMEM = {0xA1, 0xF1, ID, 0xAA, 0xAA};
      Serial.write(dataBytes, sizeof(dataBytes));
      for (int i = 0; i <= 200; i++) {
        soundFX(100.0, 30.0); // ray gun
      }
      delay(500);
    }
  }
}

void ending() {
  // Calculating score positions

  long scores[weaponNb];
  long sortScores[weaponNb];
  byte pos[weaponNb];

  if (ID == 1) {

    // Send right to send to second weapon

    data = 54;
    radio.stopListening();
    radio.openWritingPipe(addresses[1]);
    radio.write(&data, sizeof(data));

    // Receiving data

    for (int f = 2; f <= weaponNb; f++) {
      radio.startListening();
      oled.clear();
      oled.println(F("En attente"));
      while (!radio.available() && timeVal < 2000) {}
      radio.read(&data, sizeof(data));
      scores[f-1] = data;
    }
    scores[0] = score;

    // Calculations

    for (int i = 0; i <= weaponNb-1; i++) {
      sortScores[i] = scores[i];
    }

    // Sorting positions

    for(int i=0; i<(weaponNb-1); i++) {
        for(int o=0; o<(weaponNb-(i+1)); o++) {
                if(sortScores[o] > sortScores[o+1]) {
                    int t = sortScores[o];
                    sortScores[o] = sortScores[o+1];
                    sortScores[o+1] = t;
                }
        }
    }

    // Assigning positions to players

    for (int a = 0; a <= weaponNb; a++) {
      for (int b = 0; b <= weaponNb; b++) {
        if (scores[a] == sortScores[b]) {
          pos[a] = b+1;
        }
      }
    }

    // sending position

    delay(500);
    radio.stopListening();
    for (int f = 1; f <= (weaponNb); f++) {
      data = pos[f-1];;
      radio.openWritingPipe(addresses[f - 1]);
      radio.write(&data, sizeof(data));
    }
  }

  else {
    while (sendRight == false) {
      while (!radio.available()) {}
      radio.read(&data, sizeof(data));
      if (data == 54) {sendRight = true;}
    }
    radio.stopListening();
    radio.openWritingPipe(addresses[0]);
    radio.write(&score, sizeof(score));
    radio.openWritingPipe(addresses[ID]);
    data = 54;
    radio.write(&data, sizeof(data));
    radio.startListening();
    for (int i = 2; i <= weaponNb; i++) {
      while (!radio.available()) {}
      radio.read(&pos[i-1], sizeof(pos[i-1]));
    }
  }

  oled.clear();
  oled.println(F("Vous etes"));
  oled.print(pos[ID-1]);
  oled.print(F("e"));

  while (1 == 1) {}
}

void returnStart() {
  oled.clear();
  oled.println(F("Retour au"));
  oled.print(F("pt depart"));

  for (int i = 0; i < length; i++) {
    if (notes[i] == ' ') {
      delay(beats[i] * tempo); // rest
    } else {
      playNote(notes[i], beats[i] * tempo);
    }

    // pause between notes
    delay(tempo / 2);
  }
}
