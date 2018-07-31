void configuration() {

  // ID configuration

  // Reading ID and Channel in memory

  EEPROM.get(IDDef, ID);
  EEPROM.get(channelDef, channel);

  c3 = askUI("Modifier", "ID ou Freq?", 2500, c3);
  Serial.println(c3);
  if (c3 == true) {

    // ID configuration

    ID = confUI(1, ID, "ID arme", "", true, IDDef);

    // Freq confiration

    channel = confUI(1, ID, "Freq arme", "", true, channelDef);

  }

  // Configuration Weapon

  if (ID == 1) {
    c = askUI("Modifier", "config?", 3000, c);
    if (c == true) {
      c1 = askUI("Prochaine", "?", 3000, c1);
      if (c1 == true) {
        weaponNb = confUI(2, weaponNb, "Nb d'armes", "armes", false, weaponNbDef);
        gameTime = confUI(1, gameTime, "Temps", "min", false, gameTimeDef);
        scorePlus = confUI(10, scorePlus, "Score +", "pts", false, scorePlusDef);
        scoreMinus = confUI(10, scoreMinus, "Score -", "pts", false, scoreMinusDef);
        sendConf(20);
        dispMsg("Envoi");
        sendConf(weaponNb);
        sendConf(gameTime);
        sendConf(scorePlus);
        sendConf(scoreMinus);
        radio.startListening();
        waitConfirmation();
      }
      if (c1 == false) {
        c2 = askUI("Defaut", "?", 3000, c2);
      }
      if (c2 == true) {
        weaponNb = confUI(2, weaponNb, "Nb d'armes", "armes", true, weaponNbDef);
        gameTime = confUI(1, gameTime, "Temps", "min", true, gameTimeDef);
        scorePlus = confUI(10, scorePlus, "Score +", "pts", true, scorePlusDef);
        scoreMinus = confUI(10, scoreMinus, "Score -", "pts", true, scoreMinusDef);
        sendConf(22);
        dispMsg("En attente");
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
      dispMsg("Configure !");
      delay(2000);
      EEPROM.get(weaponNbDef, weaponNb);
      EEPROM.get(gameTimeDef, gameTime);
      EEPROM.get(scorePlusDef, scorePlus);;
      EEPROM.get(scoreMinusDef, scoreMinus);
      dispMsg("Envoi");
      sendConf(21);
      waitConfirmation();
    }
  }

  // Generic Weapon

  else {
    while (c == false) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("En attente");
      display.print("de conf");
      display.display();
      while (!radio.available()) {}
      radio.read(&data, sizeof(data));
      if (data == 20 || data == 21 || data == 22) {
        if (data == 21) {
          dispMsg("Par defaut");
          EEPROM.get(weaponNbDef, weaponNb);
          EEPROM.get(gameTimeDef, gameTime);
          EEPROM.get(scorePlusDef, scorePlus);;
          EEPROM.get(scoreMinusDef, scoreMinus);
          dispMsg("Configure !");
        }
        else if (data == 20) {
          dispMsg("Provisoire");
          waitData(weaponNb, false, weaponNbDef);
          waitData(gameTime, false, gameTimeDef);
          waitData(scorePlus, false, scorePlusDef);
          waitData(scoreMinus, false, scoreMinusDef);
          dispMsg("Configure !");
        }
        else if (data == 22) {
          dispMsg("Definitive");
          waitData(weaponNb, true, weaponNbDef);
          waitData(gameTime, true, gameTimeDef);
          waitData(scorePlus, true, scorePlusDef);
          waitData(scoreMinus, true, scoreMinusDef);
          dispMsg("Configure !");

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
