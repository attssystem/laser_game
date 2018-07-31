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
const byte addresses[][6] = {"00001", "00002", "00003", "00004", "00005", "00006"};

// Generic requirements

#include <EEPROM.h>
#include <ArduinoSort.h>

// Vars initialisation

unsigned long gamePlay;
int data;
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
#define weaponNbDef 0
#define gameTimeDef 1
#define scorePlusDef 2
#define scoreMinusDef 3
#define IDDef 4
#define channelDef 5
#define trigger 7
#define speakerPin 9
byte receivedID;
int score = 0;
unsigned long timeStart;
unsigned long timeVal;
int delayTime;

// Functions requirements

#include <screen.h>
#include <functions.h>
#include <sound.h>
#include <configuration.h>

// IR Requirements

byte dataBytes[5] = {0xA1, 0xF1, ID, 0xAA, 0xAA};

// Setup code

void setup() {

  // Screen configuration

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // I2C screen address
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  draw("logo", 1500);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("ID ");
  display.println(ID);
  display.display();
  delay(1500);

  // Generic configuration (Inputs-Outputs)

  pinMode(trigger, INPUT_PULLUP );
  pinMode(speakerPin, OUTPUT);

  // Parameters configuration

  configuration();

  // IR Rx-Tx configuration

  Serial.begin(9600);
  byte dataBytes[5] = {0xA1, 0xF1, ID, 0xAA, 0xAA};

  // nRF24 configuration

  addR = ID - 1;
  radio.begin();
  radio.setChannel(channel);
  radio.openReadingPipe(1, addresses[addR]);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

// Loop code

void loop() {
  configuration();
  play();
  returnStart();
  ending();
}

void play() {
  display.setTextSize(4);
  timeStart = millis();
  gamePlay = gameTime * 60000;

  // Playing during the game time

  while (timeVal < 60000) {
    //Serial.println(timeVal);
    timeVal = millis() - timeStart;
    updateDisplay();

    // If bullet received

    if (Serial.available()) {
      receivedID = Serial.read();
      if (receivedID != ID && receivedID <= weaponNb+1) {
        data = 44;
        addT = receivedID - 1;
        radio.openWritingPipe(addresses[addT]);
        radio.stopListening();
        radio.write(&data, sizeof(data));
        delay(100);
        radio.startListening();
        score = score - scoreMinus;
        draw("cross", 500);
        updateDisplay();
      }
    }

    // If signal received from nRF24

    if (radio.available()) {
      radio.read(&data, sizeof(data));
      if (data == 44) {
        draw("tick", 500);
        score = score + scorePlus;
        updateDisplay();
      }
    }

    // If trigger is pushed

    if (digitalRead(trigger) == LOW) {
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

  int scores[weaponNb + 1];
  int sortScores[weaponNb + 1];
  int pos[weaponNb + 1];
  scores[0] = -9999999;

  if (ID == 1) {
    pos[ID] = 1234;
    // receiving data

    for (int f = 2; f <= weaponNb; f++) {
      radio.startListening();
      timeStart = millis();
      timeVal = 0;
      while (!radio.available() && timeVal < 2000) {
        timeVal = millis() - timeStart;
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("attente");
        display.display();
      }
      radio.read(&data, sizeof(data));
      scores[f] = data;
      timeVal = millis() - timeStart;
      if (timeVal > 2000) {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("Redemarrer");
        display.display();
        while (1 == 1) {}
      }
    }
    scores[1] = score;

    // Calculations

    for (int i = 0; i <= weaponNb; i++) {
      sortScores[i] = scores[i];
    }
    sortArrayReverse(sortScores, weaponNb + 1);
    for (int a = 0; a <= weaponNb; a++) {
      for (int b = 0; b <= weaponNb; b++) {
        if (scores[a] == sortScores[b]) {
          pos[a] = b;
        }
      }
    }


    // sending position

    radio.stopListening();
    for (int f = 1; f <= (weaponNb); f++) {
      data = pos[f];;
      radio.openWritingPipe(addresses[f - 1]);
      radio.write(&data, sizeof(data));
    }
  }

  else {
    pos[ID] = 1234;
    delayTime = (ID - 1) * 250;
    delay(delayTime);
    radio.stopListening();
    data = score;
    radio.openWritingPipe(addresses[0]);
    radio.write(&data, sizeof(data));
    radio.startListening();
    while (!radio.available()) {}
    radio.read(&data, sizeof(data));
    pos[ID] = data;
  }

  display.setTextSize(2);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Vous etes");
  display.print(pos[ID]);
  display.print("e");
  display.display();
  while (1 == 1) {}
}

void returnStart() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("Retour au");
  display.print("pt depart");
  display.display();
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
