/* Demonstration of Rtc_Pcf8563 Set Time. 
 * Set the clock to a time then loop over reading time and 
 * output the time and date to the serial console.
 *
 * I used a RBBB with Arduino IDE, the pins are mapped a 
 * bit differently. Change for your hw
 * SCK - A5, SDA - A4, INT - D3/INT1
 *
 * After loading and starting the sketch, use the serial monitor
 * to see the clock output.
 * 
 * setup:  see Pcf8563 data sheet.
 *         1x 10Kohm pullup on Pin3 INT
 *         No pullups on Pin5 or Pin6 (I2C internals used)
 *         1x 0.1pf on power
 *         1x 32khz chrystal
 *
 * Joe Robertson, jmr
 * orbitalair@bellsouth.net
 */ 
#include <stdint.h>
#include <SPI.h>
#include <Wire.h>
#include <Rtc_Pcf8563.h>
#include <LCD.h>


//init the real time clock
Rtc_Pcf8563 rtc;

// millis를 이용하여 delay주기
unsigned long startMillis;
unsigned long endMillis;
unsigned long interval = 1000;

int gSec;
int gMin;
int gHour;
int gDay;
int gMonth;
int gYear;

void setup()
{
  Serial.begin(9600);

  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.begin();

  Tft.lcd_init();                                      // init TFT library
  Tft.lcd_clear_screen(BLACK);

  gSec = 0;
  gMin = 0;
  gHour = 0;
  gDay = 0;
  gMonth = 0;
  gYear = 0;
//  //clear out the registers
//  rtc.initClock();
//  //set a time to start with.
//  //day, weekday, month, century(1=1900, 0=2000), year(0-99)
//  rtc.setDate(15, 2, 2, 0, 17);
//  //hr, min, sec
//  rtc.setTime(00, 19, 30);
}

void loop()
{
  //both format functions call the internal getTime() so that the 
  //formatted strings are at the current time/date.

  startMillis = millis();
  const uint8_t * timeStr = rtc.formatTime();
  const uint8_t * dateStr = rtc.formatDate(RTCC_DATE_ASIA);

  int set = rtc.getSecond();
  int minute = rtc.getMinute();
  int hour = rtc.getHour();
  int day = rtc.getDay();
  int month = rtc.getMonth();
  int year = rtc.getYear();

  if (gMin == 0 || gMin != minute) {
    gMin = minute;
    Tft.lcd_clear_partscreen(BLACK, 35 + 16 * 4, 100, 16*2, 90);
  }

  if (gHour == 0 || gHour != hour || minute == 0) {
    gHour = hour;
    Tft.lcd_clear_partscreen(BLACK, 0, 100, 16*6, 90);
  }

  if (gDay == 0 || gDay != day) {
    gDay = day;
    Tft.lcd_clear_partscreen(BLACK, 10 + 16 * 9, 140, 16*2, 90);
  }

  if (gMonth == 0 || gMonth != month) {
    gMonth = month;
    Tft.lcd_clear_partscreen(BLACK, 10 + 16 * 6, 140, 16*2, 90);
  }

  if (gYear == 0 || gYear != year) {
    gYear = year;
    Tft.lcd_clear_partscreen(BLACK, 10, 140, 16*4, 90);
  }
  
  
  Tft.lcd_display_string(35, 100, (const uint8_t *)timeStr, DOTMATRIX_M_SLASH, CYAN);
  Tft.lcd_display_string(10, 140, (const uint8_t *)dateStr, DOTMATRIX_M_SLASH, WHITE);

  while(1) {
    endMillis = millis();

    if ((endMillis - startMillis) >= interval) {
      Tft.lcd_clear_partscreen(BLACK, 35 + 16 * 7, 100, 16*2, 90);
      break;
    }
  }
//  delay(1000);
//  
//  Tft.lcd_clear_partscreen(BLACK, 35 + 16 * 7, 100, 16*2, 90);
}




