// Test git
// line 2
#include "TM1637.h"

TM1637 tm1637(12, 14);              // CLK, DIO (D6, D5)

void setup() {
  tm1637.init();                      ///tm1637
  tm1637.set(7);
  //tm1637.clearDisplay();
  //tm1637.display(2,2);
  Serial.begin(115200); 
  Serial.println("Start");
}

void loop() {


 int8_t TimeDisp[4]; 

  for (float i = 0; i <= 9999; i++) {
    // tm1637.display(i);
    tm1637.clearDisplay();

    int razr0 = i / 1000;
    //TimeDisp[0] = razr1;
    int razr1 = (i - razr0 * 1000) / 100;
    //TimeDisp[1] = razr2;
    int razr2 = (i - razr0 * 1000 - razr1 * 100) / 10;
    //TimeDisp[2] = razr3;
    int razr3 = i - razr0 * 1000 - razr1 * 100 - razr2 * 10;
    //TimeDisp[3] = razr4;

    Serial.println("----");
    Serial.print(razr0);
    Serial.print(razr1);
    Serial.print(razr2);
    Serial.print(razr3);
    Serial.println();
   // TimeDisp[2] = i / 10;
    //TimeDisp[3] = i % 10;  

    //tm1637.display(TimeDisp);
    if (razr0 != 0) {
      tm1637.display(0,razr0);
      Serial.print("0-");
      Serial.println(razr0);
    }
    if (!(razr0 == 0 && razr1 == 0)) {
      tm1637.display(1,razr1);
      Serial.print("1-");
      Serial.println(razr1);
    }

    if (!(razr0 == 0 && razr1 == 0 && razr2 == 0)) {
      tm1637.display(2,razr2);
      Serial.print("2-");
      Serial.println(razr2);
    }

    tm1637.display(3,razr3); 
    Serial.print("3-");
    Serial.println(razr3);

    delay(2000);
  } 
}
