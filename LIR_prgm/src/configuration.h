void configuration() {

  // ID configuration

  c3 = askUI("ID/Freq?", 1500, c3);
  Serial.println(c3);
  if (c3 == true) {

    // ID configuration

    ID = confUI(1, ID, "ID arme", true, IDDef);

    // Freq configuration

    channel = confUI(1, ID, "Freq arme", true, channelDef);

  }

  // nRF24 configuration

  addR = ID - 1;
  radio.begin();
  radio.setChannel(channel);
  radio.openReadingPipe(1, addresses[addR]);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  // Configuration Weapon

  if (ID == 1) {
    c = askUI("config?", 1500, c);
    if (c == true) {
      delay(350);
      c1 = askUI("prochaine?", 1500, c1);
      if (c1 == true) {
        weaponNb = confUI(2, weaponNb, "Nb d'armes", false, weaponNbDef);
        gameTime = confUI(1, gameTime, "Temps(min)", false, gameTimeDef);
        scorePlus = confUI(10, scorePlus, "Score +", false, scorePlusDef);
        scoreMinus = confUI(10, scoreMinus, "Score -", false, scoreMinusDef);
        oled.clear();
        oled.setCursor(0, 0);
        oled.println(F("Envoi"));

        sendConf(20);
        sendConf(weaponNb);
        sendConf(gameTime);
        sendConf(scorePlus);
        sendConf(scoreMinus);
        waitConfirmation();
      }
      if (c1 == false) {
        c2 = askUI("defaut?", 1500, c2);
      }
      if (c2 == true) {
        weaponNb = confUI(2, weaponNb, "Nb d'armes", true, weaponNbDef);
        gameTime = confUI(1, gameTime, "Temps(min)", true, gameTimeDef);
        scorePlus = confUI(10, scorePlus, "Score +", true, scorePlusDef);
        scoreMinus = confUI(10, scoreMinus, "Score -", true, scoreMinusDef);
        oled.clear();
        oled.setCursor(0, 0);
        oled.println(F("Envoi"));

        sendConf(22);
        sendConf(weaponNb);
        sendConf(gameTime);
        sendConf(scorePlus);
        sendConf(scoreMinus);
        radio.startListening();
        c1 = true;
        waitConfirmation();
      }
    }
    else if (c == false || c1 == false) {
      oled.clear();
      oled.setCursor(0, 0);
      oled.println(F("Configure"));

      delay(2000);
      EEPROM.get(weaponNbDef, weaponNb);
      EEPROM.get(gameTimeDef, gameTime);
      EEPROM.get(scorePlusDef, scorePlus);;
      EEPROM.get(scoreMinusDef, scoreMinus);
      oled.clear();
      oled.setCursor(0, 0);
      oled.println(F("Envoi"));

      sendConf(21);
      waitConfirmation();
    }
  }

  // Generic Weapon

  else {
    while (c == false) {
      oled.clear();
      oled.setCursor(0, 0);
      oled.println("En attente");

      while (!radio.available()) {}
      radio.read(&data, sizeof(data));
      if (data == 20 || data == 21 || data == 22) {
        if (data == 21) {
          oled.clear();
          oled.setCursor(0, 0);
          oled.println(F("Par defaut"));

          EEPROM.get(weaponNbDef, weaponNb);
          EEPROM.get(gameTimeDef, gameTime);
          EEPROM.get(scorePlusDef, scorePlus);;
          EEPROM.get(scoreMinusDef, scoreMinus);
          oled.clear();
          oled.setCursor(0, 0);
          oled.println(F("Configure"));

        }
        else if (data == 20) {
          oled.clear();
          oled.setCursor(0, 0);
          oled.println(F("Provisoire"));

          waitData(weaponNb, false, weaponNbDef);
          waitData(gameTime, false, gameTimeDef);
          waitData(scorePlus, false, scorePlusDef);
          waitData(scoreMinus, false, scoreMinusDef);
          oled.clear();
          oled.setCursor(0, 0);
          oled.println(F("Configure"));

        }
        else if (data == 22) {
          oled.clear();
          oled.setCursor(0, 0);
          oled.println(F("Definitive"));

          waitData(weaponNb, true, weaponNbDef);
          waitData(gameTime, true, gameTimeDef);
          waitData(scorePlus, true, scorePlusDef);
          waitData(scoreMinus, true, scoreMinusDef);
          oled.clear();
          oled.setCursor(0, 0);
          oled.println(F("Configure"));


        }
        radio.openWritingPipe(addresses[0]);
        delayTime = (ID - 1) * 250;
        delay(delayTime);
        radio.stopListening();
        data = 22 + ID;
        radio.write(&data, sizeof(data));
        radio.startListening();
        c = true;
      }
    }
  }
}
