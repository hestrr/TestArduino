#include "TM1637.h"
#include <ESP8266WiFi.h>

TM1637 tm1637(12, 14); // CLK, DIO (D6, D5)

char ssid[] = "SnowFish";             //  your network SSID (name)
char pass[] = "38051686";        // your network password

void setup()
{
  tm1637.init(); ///tm1637
  tm1637.set(7);
  //tm1637.clearDisplay();
  //tm1637.display(2,2);
  Serial.begin(115200);
  Serial.println("Start");

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{

  //int8_t TimeDisp[4];

  for (float i = 0; i <= 9999; i++)
  {
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

    //tm1637.display(TimeDisp);
    if (razr0 != 0)
      tm1637.display(0, razr0);
    /*Serial.print("0-");
      Serial.println(razr0);*/

    if (!(razr0 == 0 && razr1 == 0))
      tm1637.display(1, razr1);

    if (!(razr0 == 0 && razr1 == 0 && razr2 == 0))
      tm1637.display(2, razr2);

    tm1637.display(3, razr3);

    delay(500);
  }
}
