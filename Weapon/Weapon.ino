/*****************************************************************************************************
  Weapon program - Laser game program for weapon

  Hugues Kadi - ATTSSystem - All rights reserved ©

  Schematics
                                    _______________
                        TX 31       |             |   VIN
                        RX 30       |             |   GND --- GND IR & nRF24 & Screen
                        RST 29      |             |   RST 29
                        GND         |             |   5V --- VCC IR + Laser (220 Ohm)
              Rx IR --- D2 32       |             |   A7 22
              Tx IR --- D3 1        |             |   A6 19
           CE nRF24 --- D4 2        |   Arduino   |   SCL 28 --- SCL Screen
          CSN nRF24 --- D5 9        |    Nano     |   SDA 27 --- SDA Screen
              Piezo --- D6 10       |     V3      |   A3 26
     Trigger button --- D7 11       |             |   A2 25
                        D8 12       |     Old     |   A1 24
                        D9 13       |  Bootloader |   A0 23
                        D10 14      |             |   AREF 21
         MOSI nRF24 --- D11 15      |             |   3V3 --- VCC nRF24 & Screen
         MISO nRF24 --- D12 16      |             |   SCK 17 -- SCK nRF24
                                    _______________

*****************************************************************************************************/

// nRF24 & Mirf requirements

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

// Screen requirements

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// Generic requirements

#include <Timer.h>
Timer t;
#define piezo 6
#define trigger 7

// Vars initialisation

int data;
bool playing = true;
bool configured = false;
bool cGameTime = false;
bool cScorePlus = true;
bool cScoreMinus = false;
bool cID = false;
int ID = 1;                                           // ID can be reconfigure with Control
int gameTime = 6;
int scorePlus = 50;
int scoreMinus = 50;
int receivedID;
int score = 0;

// IR Requirements

byte dataBytes[5] = {0xA1, 0xF1, ID, 0xAA, 0xAA};

// Setup code

void setup() {
  // Mirf configuration

  Mirf.cePin = 4; // CE pin
  Mirf.csnPin = 5; // CSN pin
  Mirf.spi = &MirfHardwareSpi; // Do not change
  Mirf.init();

  Mirf.channel = 1; // Channel 1 (0-128)
  Mirf.payload = sizeof(data); // Message size (32 octets max)
  Mirf.config();
  //Mirf.configRegister(RF_SETUP, 0x26);

  //Mirf.setTADDR((byte*) 1); // Tx address (Control Arduino)
  Mirf.setRADDR(ID); // Rx address (Generic Weapon)

  // IR Rx-Tx configuration

  Serial.begin(9600);

  // Screen configuration

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // I2C screen address
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.clearDisplay();
  display.println("ID :");
  display.setCursor(15, 0);
  display.println(ID);
  display.display();
  display.setCursor(0, 0);

  // Generic configuration (Inputs-Outputs)

  pinMode(trigger, INPUT);
  pinMode(piezo, OUTPUT);
}

// Loop code

void loop() {
  /*if (Mirf.dataReady()) {
    Mirf.getData((byte*) &msg);
    if (msg.cmd == "config") configuration();
    if (msg.cmd == "start") {
      if (configured == false) {
        display.clearDisplay();
        display.println("No config, retrying");
      }
      else {
        play();
      }
    }
    }
    else { */
  play();
  /*ID ++;
    delay(1000);
    display.clearDisplay();
    display.setCursor(0,0);
    display.println(ID);
    display.display();
    }*/
}



void play() {
  t.after(gameTime * 60 * 1000, gameEnd);
  while (playing == true) {
    updateDisplay();
    if (Serial.available()) {
      //ignore();
      receivedID = Serial.read();
      Serial.println(receivedID);
      display.println(receivedID, HEX);
      display.display();
      if (receivedID != ID && (receivedID == 1 || receivedID == 2 || receivedID == 3 || receivedID == 4 || receivedID == 5 || receivedID == 6)) {
        //display.println(receivedID, HEX);
        //display.display();
        Mirf.setTADDR(receivedID); // Tx address (Winner Weapon)
        data = receivedID;
        Mirf.send(data);
        while(Mirf.isSending());
        score = score - scoreMinus;
        updateDisplay();
        // delay(1000);
      }
    }
    if (Mirf.dataReady()) {
      display.println("QQ CHOSE");
      display.display();
      Mirf.getData(data);
      /*if (msg.cmd = "lose") {
        score = score - scoreMinus;
        updateDisplay();
        for (int i = 0; i<=20; i++) {
          soundFX(3000.0,30); // sonic screwdriver
        }
        }*/
      //if (reception.cmd == 1 || reception.val == ID) {
        //score = score + scorePlus;
        score = 123;
        updateDisplay();
        /*for (int i = 0; i<=20; i++) {
          soundFX(3000.0,30); // sonic screwdriver
          }*/
      //}
      cScorePlus = true;
    }
    if (1 == 1 && cScorePlus == true){//|| digitalRead(trigger) == HIGH) {
      cScorePlus = false;
      Serial.write(dataBytes, sizeof(dataBytes));
      delay(50);
      //for (int i = 0; i<=20; i++) {
      //soundFX(3000.0,30+200*(1+sin(millis()/5000))); // wonky sonic screwdriver
      //  soundFX(3000.0,30); // sonic screwdriver
      //soundFX(100.0,30.0); // ray gun
      //soundFX(3.0,30.0); // star trek
      //soundFX(1.0,30.0); // star trek high
      //}
    }
  }
}

void gameEnd() {
  playing = false;
}

void updateDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(score);
  display.display();
}

void soundFX(float amplitude, float period) {
  int uDelay = 2 + amplitude + amplitude * sin(millis() / period);
  for (int i = 0; i < 5; i++) {
    digitalWrite(piezo, HIGH);
    delayMicroseconds(uDelay);
    digitalWrite(piezo, LOW);
    delayMicroseconds(uDelay);
  }
}

void ignore() {
  receivedID = Serial.read();
  while (receivedID > 9) {
    receivedID = Serial.read();
  }
}













/* void configuration() {
  data msg; // Strcuture initialisation (for Mirf)
  display.clearDisplay();
  display.println("Config...");
  while (configured == false) {
    if (Mirf.dataReady()) {
      Mirf.getData((byte*) &msg);
      if (msg.cmd == "gameTime") {
        gameTime = msg.val;
        cGameTime = true;
        display.setCursor(0, 0);
        display.clearDisplay();
        display.println("Game Time :");
        display.setCursor(64, 0);
        display.println(gameTime);
        display.display();
        display.setCursor(0, 0);
      }
      else if (msg.cmd == "scorePlus") {
        scorePlus = msg.val;
        cScorePlus = true;
        display.setCursor(0, 0);
        display.clearDisplay();
        display.println("Score + :");
        display.setCursor(64, 0);
        display.println(scorePlus);
        display.display();
        display.setCursor(0, 0);
      }
      else if (msg.cmd == "scoreMinus") {
        scoreMinus = msg.val;
        cScoreMinus = true;
        display.setCursor(0, 0);
        display.clearDisplay();
        display.println("Score - :");
        display.setCursor(15, 0);
        display.println(scoreMinus);
        display.display();
        display.setCursor(0, 0);
      }
      else if (msg.cmd == ID) {
        if (msg.val == 0) {
          cID = true;
          display.setCursor(0, 0);
          display.clearDisplay();
          display.println("ID :");
          display.setCursor(15, 0);
          display.println(ID);
          display.display();
          display.setCursor(0, 0);
        }
        else {
          ID = msg.val;
          cID = true;
          display.setCursor(0, 0);
          display.clearDisplay();
          display.println("ID :");
          display.setCursor(15, 0);
          display.println(ID);
          display.display();
          display.setCursor(0, 0);
        }
      }
      if (cGameTime == true && cScorePlus == true && cScoreMinus == true) {
        configured = true;
        display.setCursor(0, 0);
        display.clearDisplay();
        display.println("Configured");
        display.display();
      }
    }
  }
} */
