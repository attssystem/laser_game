/***************************************************************************************************** 
Diagnostic program - try sending and receiving data over nRF24 and IR and try dplaying things on the screen

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
   Laser (220 Ohms) --- D8 12       |     Old     |   A1 24
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

// Ir Rx-TX requirements

byte dataBytes[5]={0xA1, 0xF1, 0xAA, 0xAB, 0xAC};
int inData;

// Screen requirements

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// Vars and #define initialisation

typedef struct {
  char cmd;
  int val;
} data;

bool diagnostic = true;

// Setup code

void setup() {
  // Diagnostic configuration

  Serial.begin(9600);
  
  // Mirf configuration
  
  Mirf.cePin = 4; // CE pin
  Mirf.csnPin = 5; // CSN pin
  Mirf.spi = &MirfHardwareSpi; // Do not change
  Mirf.init();

  Mirf.channel = 1; // Channel 1 (0-128)
  Mirf.payload = sizeof(data); // Message size (32 octets max)
  Mirf.config();
  Mirf.configRegister(RF_SETUP, 0x26);

  Mirf.setTADDR((byte *) "nrf01"); // Tx address
  Mirf.setRADDR((byte *) "nrf02"); // Rx address

  // Screen configuration

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // I2C screen address
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
}

// Loop code

void loop() {
  Serial.println("Diagnostic Start");

  // nRF24 & Mirf test
  
  Serial.println("Try sending data over nRF24");
  data msg;
  msg.cmd = "test";
  msg.val = 1;
  Mirf.send((byte*) &msg);
  while(Mirf.isSending());
  
  /*data msg;
  if (Mirf.dataReady()) {
    Mirf.getData((byte*) &msg);
    if (msg.cmd == "test" && msg.val == 1){
      Serial.println("nRF24 & Mirf : OK");
    }
    else {
      Serial.println("nRF24 & Mirf : Error");
      diagnostic = false;
    }
  }*/

  /*// IR Rx-Tx test

  Serial.println("Trying sending data over IR");
  Serial.write(dataBytes, sizeof(dataBytes));
  Serial.println("Data sent");
  if (Serial.available()) {
    if (Serial.read() == "AA") {
      Serial.println("IR Rx-Tx : OK");
    }
    else {
      Serial.println("IR Rx-Tx : Error");
      diagnostic = false;
    }
  }*/

  /*// Screen test

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Salut !");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.println("Salut en retour !");
  display.display();
  delay(2000);
  display.clearDisplay();
  Serial.println("Screen : OK");*/
  /*if (diagnostic == true) {
    Serial.println("Diagnostic : OK");
    display.println("Diagnostic : OK");
    display.display();
  }
  else {
    Serial.println("Diagnostic : Error");
    display.println("Diagnostic : Error");
    display.display();
  }*/
}
