#include <stdarg.h>
#define SERIAL_PRINTF_MAX_BUFF 256
void serialPrintf(const char *fmt, ...);

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <Ticker.h>

#include <Wire.h>      //I2C library
#include <RtcDS3231.h> //RTC library

RtcDS3231<TwoWire> rtc(Wire);

int pinCS = D4;
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays = 1;

Ticker flipper, getTime;

// LED Matrix Pin -> ESP8266 Pin
// Vcc            -> 3v  (3V on NodeMCU 3V3 on WEMOS)
// Gnd            -> Gnd (G on NodeMCU)
// DIN            -> D7  (Same Pin for WEMOS)
// CS             -> D4  (Same Pin for WEMOS)
// CLK            -> D5  (Same Pin for WEMOS)

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

char ssid[] = "SnowFish"; //  your network SSID (name)
char pass[] = "38051686"; // your network password
#include <ESP8266WiFi.h>

unsigned int localPort = 2390; // local port to listen for UDP packets

IPAddress timeServerIP; // time.nist.gov NTP server address
const char *ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

#include <WiFiUdp.h>
// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

#define RTC_LEAP_YEAR(year) ((((year) % 4 == 0) && ((year) % 100 != 0)) || ((year) % 400 == 0))

#define GMT 4
boolean point;

volatile boolean needRedraw = false;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("Start");

  rtc.Begin();

  matrix.setIntensity(15);  // Use a value between 0 and 15 for brightness
  matrix.setRotation(0, 1); // The first display is position upside down
  matrix.setRotation(1, 1); // The first display is position upside down
  matrix.setRotation(2, 1); // The first display is position upside down
  matrix.setRotation(3, 1); // The first display is position upside down
  matrix.fillScreen(LOW);
  matrix.write();

  matrix.drawChar(2, 0, '1', HIGH, LOW, 1);  // H
  matrix.drawChar(9, 0, '2', HIGH, LOW, 1);  // HH
  matrix.drawChar(14, 0, ':', HIGH, LOW, 1); // HH:
  matrix.drawChar(19, 0, '3', HIGH, LOW, 1); // HH:M
  matrix.drawChar(26, 0, '4', HIGH, LOW, 1); // HH:MM
  matrix.write();                            // Send bitmap to display

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());

  flipper.attach(1, flip);
  //getTime.attach(60, GetTimeFromInternet);
}

void loop()
{
  /*flip();
  delay(500);*/

  if (needRedraw)
  {
    RtcDateTime now = rtc.GetDateTime();
    byte hour = now.Hour();
    byte minute = now.Minute();

    /*int8_t TimeDisp[4]; // отправляем всё на экран
  /*char hr_1 = char(hour / 10);
  char hr_2 = str(hour % 10);
  str min_3 = str(minute / 10);
  str min_4 = str(minute % 10);*/

    matrix.drawChar(2, 0, String(hour / 10)[0], HIGH, LOW, 1);    // H
    matrix.drawChar(9, 0, String(hour % 10)[0], HIGH, LOW, 1);    // HH
    matrix.drawChar(14, 0, point ? ':' : ' ', HIGH, LOW, 1);      // HH:
    matrix.drawChar(19, 0, String(minute / 10)[0], HIGH, LOW, 1); // HH:M
    matrix.drawChar(26, 0, String(minute % 10)[0], HIGH, LOW, 1); // HH:MM
    matrix.write();                                               // Send bitmap to display
    needRedraw = false;
  }
}

void GetTimeFromInternet()
{
  //get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP);

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);

  int cb = udp.parsePacket();
  if (!cb)
  {
    Serial.println("no packet yet");
  }
  else
  {
    //Serial.print("packet received, length=");
    //Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    //Serial.print("Seconds since Jan 1 1900 = ");
    //Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:

    /// корректировка часового пояса и синхронизация
    unsigned long unix = epoch + GMT * 3600;
    Serial.println(unix);

    byte ss = unix % 60; /* Get seconds from unix */
    unix /= 60;          /* Go to minutes */
    byte mm = unix % 60; /* Get minutes */
    unix /= 60;          /* Go to hours */
    byte hh = unix % 24; /* Get hours */
    unix /= 24;          /* Go to days */
    Serial.print(hh);
    Serial.print(":");
    Serial.print(mm);
    Serial.print(":");
    Serial.print(ss);
    Serial.print("   ");
    // data->WeekDay = (unix + 3) % 7 + 1; /* Get week day, monday is first day */

    byte WeekDay = (unix + 3) % 7 + 1; /* Get week day, monday is first day */

    byte year = 1970; /* Process year */
    while (1)
    {
      if (RTC_LEAP_YEAR(year))
      {
        if (unix >= 366)
          unix -= 366;
        else
          break;
      }
      else if (unix >= 365)
        unix -= 365;
      else
        break;
      year++;
    }
    /* Get year in xx format */
    year = (uint8_t)(year - 2000);
    Serial.print(year);
    Serial.print("-");

    /* Get month */
    static uint8_t RTC_Months[2][12] = {
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, /* Not leap year */
        {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}  /* Leap year */
    };

    byte month;
    for (month = 0; month < 12; month++)
    {
      if (RTC_LEAP_YEAR(year))
      {
        if (unix >= (uint32_t)RTC_Months[1][month])
          unix -= RTC_Months[1][month];
        else
          break;
      }
      else if (unix >= (uint32_t)RTC_Months[0][month])
        unix -= RTC_Months[0][month];
      else
        break;
    }

    Serial.print(month + 1);
    Serial.print("-");
    Serial.print(unix);
    Serial.println("");

    RtcDateTime dateTime(year, month + 1, unix, hh, mm, ss);
    rtc.SetDateTime(dateTime);

    /*

    byte hour = (epoch % 86400L) / 3600;
    byte minute = (epoch % 3600) / 60;
    byte second = epoch % 60;

    // print the hour, minute and second:
    Serial.print("The UTC time is ");      // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if (((epoch % 3600) / 60) < 10)
    {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ((epoch % 60) < 10)
    {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second*/
  }
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress &address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0;          // Stratum, or type of clock
  packetBuffer[2] = 6;          // Polling Interval
  packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void flip()
{
  point = !point;

  needRedraw = true;

  /*second++;
  if (second > 59)
  {
    second = 0;
    minute++;
  }
  if (minute > 59)
  {
    minute = 0;
    hour++;
  }
  if (hour > 23)
  {
    hour = 0;
  }*/
}