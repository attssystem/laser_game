/***************************************************************************************************** 
Control program - Laser game program for control

Hugues Kadi - ATTSSystem - All rights reserved ©

Schematics
                                    _______________
                        TX 31       |             |   VIN
                        RX 30       |             |   GND --- GND IR & nRF24 & Screen
                        RST 29      |             |   RST 29
                        GND         |             |   5V --- VCC IR
              Rx IR --- D2 32       |             |   A7 22
              Tx IR --- D3 1        |             |   A6 19
           CE nRF24 --- D4 2        |   Arduino   |   SCL 28 --- SCL Screen
          CSN nRF24 --- D5 9        |    Nano     |   SDA 27 --- SDA Screen
              Piezo --- D6 10       |     V3      |   A3 26
     Trigger button --- D7 11       |             |   A2 25
   Laser (250 Ohms) --- D8 12       |     Old     |   A1 24
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

// Vars initialisation

typedef struct {
  char cmd;
  int val;
} data;
bool playing = true;
bool configured = false;
bool cGameTime = false;
bool cScorePlus = false;
bool cScoreMinus = false;
bool cID = false;
int ID = 1;                                           // ID can be reconfigure with Control
int gameTime;
int scorePlus;
int scoreMinus;
int receivedID;
int score;

// Setup code

void setup() {
  // Mirf configuration
  
  Mirf.cePin = 2; // CE pin
  Mirf.csnPin = 9; // CSN pin
  Mirf.spi = &MirfHardwareSpi; // Do not change
  Mirf.init();

  Mirf.channel = 1; // Channel 1 (0-128)
  Mirf.payload = sizeof(data); // Message size (32 octets max)
  Mirf.config();
  Mirf.configRegister(RF_SETUP, 0x26);

  Mirf.setTADDR((byte *) "Wpn"); // Tx address (Generic Weapon)
  Mirf.setRADDR((byte *) "Ctrl"); // Rx address (Control Arduino)
}

// Loop code

void loop() {
  data msg; // Strcuture initialisation (for Mirf)

  // Config Interface

  // ID Config (If not configured, cID = false)
  
  msg.cmd = "config";
  msg.val = 1;
  Mirf.send((byte*) &msg);
  delay(500);
  msg.cmd = "gameTime";
  msg.val = gameTime;
  Mirf.send((byte*) &msg);
  delay(500);
  msg.cmd = "scorePlus";
  msg.val = scorePlus;
  Mirf.send((byte*) &msg);
  delay(500);
  msg.cmd = "scoreMinus";
  msg.val = scoreMinus;
  Mirf.send((byte*) &msg);
  delay(500);
  msg.cmd = "ID";
  if (cID == false) msg.val = 0;
  else {
    // ID Config
  }
}
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  if (Mirf.dataReady()) {
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
}

void configuration() {
  data msg; // Strcuture initialisation (for Mirf)
  display.clearDisplay();
  display.println("Config...");
  while (configured == false) {
      if (Mirf.dataReady()) {
          Mirf.getData((byte*) &msg);
          if (msg.cmd == "gameTime") {
            gameTime = msg.val;
            cGameTime = true;
            display.setCursor(0,0);
            display.clearDisplay();
            display.println("Game Time :");
            display.setCursor(64,0);
            display.println(gameTime);
            display.setCursor(0,0);
          }
          else if (msg.cmd == "scorePlus") {
            scorePlus = msg.val;
            cScorePlus = true;
            display.setCursor(0,0);
            display.clearDisplay();
            display.println("Score + :");
            display.setCursor(64,0);
            display.println(scorePlus);
            display.setCursor(0,0);
          }
          else if (msg.cmd == "scoreMinus") {
            scoreMinus = msg.val;
            cScoreMinus = true;
            display.setCursor(0,0);
            display.clearDisplay();
            display.println("Score - :");
            display.setCursor(15,0);
            display.println(scoreMinus);
            display.setCursor(0,0);
          }
          else if (msg.cmd == ID) {
            if (msg.val == 0) {
              cID = true;
              display.setCursor(0,0);
              display.clearDisplay();
              display.println("ID :");
              display.setCursor(15,0);
              display.println(ID);
              display.setCursor(0,0);
            }
            else {
              ID = msg.val;  
              cID = true;
              display.setCursor(0,0);
              display.clearDisplay();
              display.println("ID :");
              display.setCursor(15,0);
              display.println(ID);
              display.setCursor(0,0);
            }
          }
          if (cGameTime == true && cScorePlus == true && cScoreMinus == true) {
            configured = true;
            display.setCursor(0,0);
            display.clearDisplay();
            display.println("Configured");
      }
    }
  }
}

void play() {
  data msg;
  t.after(gameTime*60*1000, gameEnd);
  while (play == true) {
    if (digitalRead(trigger) == HIGH) {
      IR.print(ID);
      for (int i = 0; i<=20; i++) {
          //soundFX(3000.0,30+200*(1+sin(millis()/5000))); // wonky sonic screwdriver
          soundFX(3000.0,30); // sonic screwdriver
          //soundFX(100.0,30.0); // ray gun
          //soundFX(3.0,30.0); // star trek
          //soundFX(1.0,30.0); // star trek high
      }
    }
    if (IR.available()) {
      receivedID = IR.read();
      msg.cmd = "win";
      msg.val = receivedID;
      Mirf.send((byte*) &msg);
    }
    if (Mirf.dataReady()) {
      Mirf.getData((byte*) &msg);
      if (msg.cmd = "lose") {
        score = score - scoreMinus;
        updateDisplay();
        for (int i = 0; i<=20; i++) {
          soundFX(3000.0,30); // sonic screwdriver
        }
      }
      if (msg.cmd = "win") {
        score = score + scorePlus;
        updateDisplay();
        for (int i = 0; i<=20; i++) {
          soundFX(3000.0,30); // sonic screwdriver
        }
      }
    }
  }
}

void gameEnd() {
  playing = false;
}

void updateDisplay() {
  display.clearDisplay();
  display.println(score);
}

void soundFX(float amplitude,float period){ 
 int uDelay=2+amplitude+amplitude*sin(millis()/period);
 for(int i=0;i<5;i++){
   digitalWrite(piezo,HIGH);
   delayMicroseconds(uDelay);
   digitalWrite(piezo,LOW);
   delayMicroseconds(uDelay);
 }
} 
