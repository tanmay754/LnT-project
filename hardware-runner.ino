#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

#define TEMP_PIN    A0  
#define BUZZER_PIN  7    

const float TEMP_LIMIT = 30.0; 
const float BETA       = 3950.0;

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Wire.begin();
  rtc.begin();

  lcd.init();
  lcd.backlight();
}

void loop() {
  DateTime now = rtc.now();
  int analogValue = analogRead(TEMP_PIN);
  float tempC;
  if (analogValue <= 0 || analogValue >= 1023) {
    tempC = NAN;
  } else {
    tempC = 1.0 / (
      log(1.0 / (1023.0 / analogValue - 1.0)) / BETA
      + 1.0 / 298.15
    ) - 273.15;
  }
  lcd.clear();
  lcd.setCursor(0, 0);

  if (now.hour() < 10) lcd.print('0');
  lcd.print(now.hour());
  lcd.print(':');

  if (now.minute() < 10) lcd.print('0');
  lcd.print(now.minute());
  lcd.print(':');

  if (now.second() < 10) lcd.print('0');
  lcd.print(now.second());

  char tbuf[6];
  if (isnan(tempC)) {
    strcpy(tbuf, "--.-");
  } else {
    dtostrf(tempC, 4, 1, tbuf); 
  }

  lcd.setCursor(9, 0);
  lcd.print(tbuf);
  lcd.write(0b11011111);  
  lcd.print('C');
  lcd.setCursor(0, 1);
  if (!isnan(tempC) && tempC > TEMP_LIMIT) {
    lcd.print("! OVERHEAT !   ");
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    lcd.print("Temp OK        ");
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(1000);
}



