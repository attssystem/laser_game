void waitConfirmation() {
  for (int f = 24; f <= weaponNb + 22; f++) {
    oled.clear();
    oled.setCursor(0, 0);
    oled.println(F("En attente"));

    timeStart = millis();
    timeVal = 0;
    while (!radio.available() && timeVal < 2000) {
      timeVal = millis() - timeStart;
    }
    radio.read(&data, sizeof(data));
    timeVal = millis() - timeStart;
    if (data != 24 || timeVal > 2000) {
      oled.clear();
      oled.setCursor(0, 0);
      oled.println(F("Redemarrer"));

      while (1 == 1) {}
    }
  }
}
byte confUI(byte def, byte val, char char1[10], bool EEPR, byte valDef) {
  val = def;
  oled.clear();
  oled.setCursor(0, 0);
  oled.println(char1);
  oled.println(val);

  delay(350);
  timeStart = millis();
  timeVal = millis() - timeStart;
  while (timeVal < 2000) {
    timeVal = millis() - timeStart;
    if (digitalRead(trigger) == LOW) {
      val++;
      delay(250);
      timeStart = millis();
      oled.clear();
      oled.setCursor(0, 0);
      oled.println(char1);
      oled.println(val);

    }
  }
  if (EEPR == true) {
    EEPROM.write(valDef, val);
  }
  return val;
}

void sendConf(int d) {
  radio.stopListening();
  for (int h = 1; h <= (weaponNb - 1); h++) {
    data = d;
    radio.openWritingPipe(addresses[h]);
    radio.write(&data, sizeof(data));
  }
  radio.startListening();
}

void waitData(int val, bool EEPR, int valDef) {
  timeStart = millis();
  timeVal = millis() - timeStart;
  while (!radio.available() && timeVal < 260) {
    timeVal = millis() - timeStart;
  }
  if (timeVal > 260) {
    oled.clear();
    oled.setCursor(0, 0);
    oled.println(F("Redemarrer"));

    while (1 == 1) {}
  }
  radio.read(&data, sizeof(data));
  val = data;
  if (EEPR == true) {
    EEPROM.put(valDef, data);
  }
}

bool askUI (char char1[], int wait, int configurable) {
  oled.clear();
  oled.setCursor(0, 0);
  oled.println(F("Modifier"));
  oled.println(char1);

  timeStart = millis();
  timeVal = 0;
  while (timeVal < wait && configurable == false) {
    Serial.println(digitalRead(trigger));
    timeVal = millis() - timeStart;
    if (digitalRead(trigger) == LOW) {
      configurable = true;
    }
    else {configurable = false;}
  }
  return configurable;
}

void updateDisplay() {
  oled.clear();
  oled.set2X();
  oled.setCursor(0, 0);
  oled.println(score);

}
