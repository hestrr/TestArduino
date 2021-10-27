// Test git
// line 2
#include "TM1637.h"

TM1637 tm1637(12, 14);              // CLK, DIO (D6, D5)

void setup() {
  tm1637.init();                      ///tm1637
  tm1637.set(7);
}

void loop() {
  float i = 1234;
  tm1637.display(i);
}
