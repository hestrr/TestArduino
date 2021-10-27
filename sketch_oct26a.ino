// Test git
// line 2
#include "TM1637.h"

TM1637 tm1637(12, 14);              // CLK, DIO (D6, D5)

void setup() {
  tm1637.init();                      ///tm1637
  tm1637.set(7);
}

void loop() {
  //float i = 1234;
 // tm1637.display(i);

 int8_t TimeDisp[4]; 

  for (float i = 0; i <= 9999; i++) {
    // tm1637.display(i);
    int razr1 = i / 1000;
    TimeDisp[0] = razr1;
    int razr2 = (i - razr1 * 1000) / 100;
    TimeDisp[1] = razr2;
    int razr3 = (i - razr1 * 1000 - razr2 * 100) / 10;
    TimeDisp[2] = razr3;
    int razr4 = i - razr1 * 1000 - razr2 * 100 - razr3 * 10;
    TimeDisp[3] = razr4;

   // TimeDisp[2] = i / 10;
    //TimeDisp[3] = i % 10;  

    tm1637.display(TimeDisp);

    delay(500);
  }
}
