void dispMsg(char char1[]) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(char1);
  display.display();
}

void waitConfirmation() {
  for (int f = 24; f <= weaponNb + 22; f++) {
    dispMsg("En attente");
    timeStart = millis();
    timeVal = 0;
    while (!radio.available() && timeVal < 2000) {
      timeVal = millis() - timeStart;
    }
    radio.read(&data, sizeof(data));
    timeVal = millis() - timeStart;
    if (data != 24 || timeVal > 2000) {
      dispMsg("Redemarrer");
      while (1 == 1) {}
    }
  }
}
byte confUI(int def, byte val, char char1[], char char2[], bool EEPR, int valDef) {
  val = def;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(char1);
  display.setCursor(0, 16);
  display.println(val);
  display.setCursor(64, 16);
  display.println(char2);
  display.display();
  delay(350);
  timeStart = millis();
  timeVal = millis() - timeStart;
  while (timeVal < 2000) {
    timeVal = millis() - timeStart;
    if (digitalRead(trigger) == LOW) {
      val++;
      delay(250);
      timeStart = millis();
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(char1);
      display.setCursor(0, 16);
      display.println(val);
      display.setCursor(64, 16);
      display.println(char2);
      display.display();
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
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Redemarrer");
    display.display();
    while (1 == 1) {}
  }
  radio.read(&data, sizeof(data));
  val = data;
  if (EEPR == true) {
    EEPROM.put(valDef, data);
  }
}

bool askUI (char char1[], char char2[], int wait, int configurable) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(char1);
  display.println(char2);
  display.display();
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
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(score);
  display.display();
}

void draw(const unsigned char image [] PROGMEM, int duration) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.drawBitmap(0, 0, image, 128, 32, WHITE);
  display.display();
  delay(duration);
}
