#include <Arduino.h>
#include <ST7032.h>

#define CHARGING_PIN 4
#define LIMIT 2
#define REMIND 1
#define FREQ 10000 //ms

ST7032 lcd;

const PROGMEM String upperLine = "Welcome ";
const PROGMEM String lowerLine = "   30min";

uint8_t time = 0;
int start = 0;
bool state = false; // true: stay, false: charging
bool remind = false;

void alerm(){
  for(int i = 0; i < 3; i++){
    tone(12, 988, 50);
    delay(100);
  }
}

String limitText(uint8_t lim){
  String brank = "";
  String limStr = String(lim);
  for(int i = 0; i < 5 - limStr.length(); i++) brank += " ";
  return brank + String(lim) + "min";
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

  updateLcd(upperLine);
  updateLcd(lowerLine, false);
}

void setup(){
  Serial.begin(115200);
  lcdInitialize();
}

void loop(){
  int now = millis();
  bool pinState = digitalRead(CHARGING_PIN); //true: stay, false: charging

  if(state){
    if(!pinState){
      time = 0;
      state = false;
      remind = false;
    }else{
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
        alerm();
        delay(500);
      }
    }else{
      if(time == 1 && !remind){
        alerm();
        remind = true;
      }
      updateLcd(limitText(time), false);
    }
    delay(1000);
  }else{
    updateLcd("Charging", false);
  }
}
