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
  data = d;
  radio.openWritingPipe(addresses[1]);
  radio.write(&data, sizeof(data));
  delay(50);
}

byte waitData(int val, bool EEPR, int valDef) {
  while (!radio.available()) {}
  radio.read(&data, sizeof(data));
  if (EEPR == true) {
    EEPROM.put(valDef, data);
  }
  return data;
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
  oled.print("Score :");
  oled.setCursor(0, 2);
  oled.print(score);
  oled.setCursor(0, 4);
  oled.print("Tps restant :");
  oled.setCursor(0, 6);
  oled.print(oldMinutes);
  oled.setCursor(30, 6);
  oled.print("min");
  oled.setCursor(70, 6);
  oled.print(oldSeconds);
  oled.setCursor(100, 6);
  oled.print("s");
}

byte updateTime() {
  timeRemain = gameTime * 60000 - timeVal;
  newSeconds = timeRemain/1000-(newMinutes*60);
  if (newSeconds > 60) {
    newSeconds = newSeconds - 60;
  }
  newMinutes = timeRemain/60000;
  if (newSeconds != oldSeconds) {
    oldSeconds = newSeconds;
    updateDisplay();
  }
  else if (newMinutes != oldMinutes) {
    oldMinutes = newMinutes;
    updateDisplay();
  }
}
