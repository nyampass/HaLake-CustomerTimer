#include <Arduino.h>
#include <ST7032.h>

#define CHARGING_PIN A3
#define TONE_PIN 13
#define LIMIT 120 //min
#define REMIND 5 //min
#define FREQ 60000 //ms

ST7032 lcd;

const PROGMEM String upperLineStay = " Remains";
const PROGMEM String upperLineCharging = "Welcome!";
const PROGMEM String lowerLine = "  200min";

uint8_t time = 0;
unsigned long start = 0;
bool state = false; // true: stay, false: charging
bool remind = false;

void alarm(){
  for(int i = 0; i < 3; i++){
    tone(TONE_PIN, 988, 50);
    delay(100);
  }
}

String limitText(uint8_t lim){
  String blank = "";
  String limStr = String(lim);
  for(int i = 0; i < 5 - limStr.length(); i++) blank += " ";
  return blank + String(lim) + "min";
}

void writeChar(uint8_t x, uint8_t y, char c){
  lcd.setCursor(x, y);
  lcd.print(c);
}

void updateLcd(String str, bool pos = true){ // pos=true: upper, pos=false: lower
  for(int i = 0; i < str.length(); i++) writeChar(i, !pos, str[i]);
}

void lcdInitialize(){
  lcd.begin(8, 2);
  lcd.setContrast(30);

  updateLcd(upperLineCharging);
  updateLcd(lowerLine, false);
}

void setup(){
  Serial.begin(115200);
  lcdInitialize();
}

void loop(){
  unsigned long now = millis();
  bool pinState = digitalRead(CHARGING_PIN); //true: stay, false: charging

  if(state){
    if(!pinState){
      time = 0;
      state = false;
      remind = false;
    }else{
      Serial.println(now - start);
      if(now - start > FREQ){
        time -= 1;
        start = now;
      }
    }
  }else{
    if(pinState){
      start = now;
      time = LIMIT;
      state = true;
    }
  }

  if(state){
    if(time == 0){
      updateLcd("    Over", false);
      while(digitalRead(CHARGING_PIN)){
        alarm();
        delay(500);
      }
    }else{
      if(time == REMIND && !remind){
        alarm();
        remind = true;
      }
      updateLcd(upperLineStay, true);
      updateLcd(limitText(time), false);
    }
  }else{
    updateLcd(upperLineCharging, true);
    updateLcd("Charging", false);
  }
  delay(1);
}
