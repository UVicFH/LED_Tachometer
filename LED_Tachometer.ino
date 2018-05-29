/* SparkFun WS2812 Breakout Board Example
  SparkFun Electronics
  date: July 25, 2013
  license: GNU GENERAL PUBLIC LICENSE
  
  Requires the Adafruit NeoPixel library. It's awesome, go get it.
  https://github.com/adafruit/Adafruit_NeoPixel
  
  This simple example code runs three sets of animations on a group of WS2812
  breakout boards. The more boards you link up, the better these animations
  will look. 
  
  For help linking WS2812 breakouts, checkout our hookup guide:
  https://learn.sparkfun.com/tutorials/ws2812-breakout-hookup-guide
  
  Before uploading the code, make sure you adjust the two defines at the
  top of this sketch: PIN and LED_COUNT. Pin should be the Arduino pin
  you've got connected to the first pixel's DIN pin. By default it's
  set to Arduino pin 4. LED_COUNT should be the number of breakout boards
  you have linked up.
*/
#include "Adafruit_NeoPixel.h"
#include "WS2812_Definitions.h"

#define PIN 4
#define LED_COUNT 66

#include <SPI.h>
#include "mcp_can.h"

const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin


// Create an instance of the Adafruit_NeoPixel class called "leds".
// That'll be what we refer to from here on...
Adafruit_NeoPixel leds = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  leds.begin();  // Call this to start up the LED strip.
  clearLEDs();   // This function, defined below, turns all LEDs off...
  leds.show();   // ...but the LEDs don't actually update until you call this.
   
  Serial.begin(115200);

START_INIT:

  if(CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
  {
      Serial.println("CAN BUS Shield init ok!");
  }
  else
  {
      Serial.println("CAN BUS Shield init fail");
      Serial.println("Init CAN BUS Shield again");
      delay(100);
      goto START_INIT;
  }

  /*
   * set mask, set both the mask to 0x3ff
   */
  CAN.init_Mask(0, 0, 0xFF);                         // there are 2 mask in mcp2515, you need to set both of them
  CAN.init_Mask(1, 0, 0xFF);

  /*
   * set filter, we can receive id from 0x04 ~ 0x09
   */
  CAN.init_Filt(0, 0, 1520);                          // there are 6 filter in mcp2515
  CAN.init_Filt(1, 0, 1520);                          // there are 6 filter in mcp2515

  CAN.init_Filt(2, 0, 1520);                          // there are 6 filter in mcp2515
  CAN.init_Filt(3, 0, 1520);                          // there are 6 filter in mcp2515
  CAN.init_Filt(4, 0, 1520);                          // there are 6 filter in mcp2515
  CAN.init_Filt(5, 0, 1520);                          // there are 6 filter in mcp2515
  
}

float cur_rpm = 5000;
float max_rpm = 12300;
float flash_rpm = 11500;
int flash_speed = 50;
int color = 0;
float rpm_led = max_rpm/LED_COUNT;

uint32_t cur_led_color = GREEN;

void loop(){

  unsigned char len = 0;
  unsigned char buf[8];

  if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
  {
      CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

      unsigned long canId = CAN.getCanId();
      
      if(canId == 1520){
        set_color(buf[6] << 8 | buf[7]);
      }
  }

  if (cur_rpm > flash_rpm){
    
    clearLEDs();
    leds.show();
    delay(flash_speed);
    set_color(buf[6] << 8 | buf[7]);
    delay(flash_speed);
 }

}

void set_color(int rpm){
  
  cur_rpm = rpm;  
  
  clearLEDs();
  cur_led_color = HSBtoRGB(int(120*(1.0-float(cur_rpm/max_rpm))),1,1);
  
  //Serial.println(120*(float(cur_rpm/max_rpm)));
  
  for(int i=LED_COUNT; i>0; i--){
    if ( cur_rpm >= (LED_COUNT-i)*rpm_led+1){
      
      leds.setPixelColor(i, cur_led_color);
      leds.setBrightness(255);
    }
    
  }

  leds.show();
}

// Sets all LEDs to off, but DOES NOT update the display;
// call leds.show() to actually turn them off after this.
void clearLEDs()
{
  for (int i=0; i<LED_COUNT; i++)
  {
    leds.setPixelColor(i, 0);
  }
}

long HSBtoRGB(float _hue, float _sat, float _brightness) {
   float red = 0.0;
   float green = 0.0;
   float blue = 0.0;
   
   if (_sat == 0.0) {
       red = _brightness;
       green = _brightness;
       blue = _brightness;
   } else {
       if (_hue == 360.0) {
           _hue = 0;
       }

       int slice = _hue / 60.0;
       float hue_frac = (_hue / 60.0) - slice;

       float aa = _brightness * (1.0 - _sat);
       float bb = _brightness * (1.0 - _sat * hue_frac);
       float cc = _brightness * (1.0 - _sat * (1.0 - hue_frac));
       
       switch(slice) {
           case 0:
               red = _brightness;
               green = cc;
               blue = aa;
               break;
           case 1:
               red = bb;
               green = _brightness;
               blue = aa;
               break;
           case 2:
               red = aa;
               green = _brightness;
               blue = cc;
               break;
           case 3:
               red = aa;
               green = bb;
               blue = _brightness;
               break;
           case 4:
               red = cc;
               green = aa;
               blue = _brightness;
               break;
           case 5:
               red = _brightness;
               green = aa;
               blue = bb;
               break;
           default:
               red = 0.0;
               green = 0.0;
               blue = 0.0;
               break;
       }
   }

   long ired = red * 255.0;
   long igreen = green * 255.0;
   long iblue = blue * 255.0;
   
   return long((ired << 16) | (igreen << 8) | (iblue));
}
