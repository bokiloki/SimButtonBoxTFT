#include <Adafruit_TFTLCD.h>
#include <pin_magic.h>
#include <registers.h>

#include <TouchScreen.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#include <Joystick.h>


#if defined(__SAM3X8E__)
#undef __FlashStringHelper::F(string_literal)
#define F(string_literal) string_literal
#endif



TouchScreen ts = TouchScreen(6, A1, A2, 7, 274);

#define  B   0xF800

#define G   0x07E0

#define W  0xFFFF

#define R   0xFBE0

Joystick_ Joystick;

Adafruit_TFTLCD tft(A3, A2, A1, A0, A4);
void setup() {
  tft.reset();
  uint16_t identifier = 0x9341;
  tft.begin(identifier);
  Joystick.begin();
  Serial.begin(115200);
  Joystick.setRxAxisRange(0, 100);
  Joystick.setRyAxisRange(0, 100);
  Joystick.setRzAxisRange(0, 100);
  Joystick.setXAxisRange(0, 100);
  Joystick.setYAxisRange(0, 100);
  Joystick.setZAxisRange(0, 100);
  Joystick.setThrottleRange(0, 100);
  Joystick.setRxAxis(0);
  Joystick.setRyAxis(0);
  Joystick.setRzAxis(0);
  Joystick.setXAxis(0);
  Joystick.setYAxis(0);
  Joystick.setZAxis(0);
  Joystick.setThrottle(0);
  displayButtons(0);
}
#define mi 10
#define ma 1000
int controllers = 0;
void loop() {
  touch();
}
int buttons[48] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int lengthXrYrZrXYZTh[8] = {0, 0, 0, 0, 0, 0, 0,50};
char charji[3] = {'A', 'B', 'C'};
String buttonstext[48] = {"01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31", "32", "-", "+", "-", "+", "-", "+", "-", "+", "-", "+", "-", "+", "-", "+", "B", "C"};
void touch()
{
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  delay(10);
  digitalWrite(13, LOW);
  pinMode(A2, OUTPUT);
  pinMode(A1, OUTPUT);
  int lastpz = p.z;
  if (p.z > mi && p.z < ma)
  {

    p.x = tft.width() - (map(p.x, 150, 920, tft.width(), 0));
    p.y = tft.height() - (map(p.y, 120, 940, tft.height(), 0));
    //
    if (p.y < 80 && p.y > 0)
    {
      for (int i = 0; i <= 160; i += 80)
      {
        if ((p.x < i + 80 && p.x > i))
        {
          displayButtons(i / 80 * 16);
        }
      }
    }
    else if (controllers == 0 || controllers == 1)
    {
      for (int i = 80, buttoncounter = controllers * 16; i <= 260; i += 60, buttoncounter++)
      {
        for (int j = 0; j < 240; j += 60, buttoncounter++)
        {

          if ((p.x < j + 60 && p.x > j) && (p.y < i + 60 && p.y > i))
          {
            if (buttons[buttoncounter] == 0)
            {
              tft.fillRect(j + 1, i + 1, 58, 58, G);
              Joystick.setButton(buttoncounter, 1);
              buttons[buttoncounter] = 1;
              int count = 0;
              int lastpzavg = lastpz;
              while (lastpzavg != 0)
              {
                if (count < 10) {
                  p = ts.getPoint();
                  delay(10);
                  lastpz += p.z;
                  count++;
                }
                else {
                  lastpzavg = lastpz / count;
                  count = 0;
                  lastpz = 0;
                }

              }
              Joystick.setButton(buttoncounter, 0);
            }



          }



        } buttoncounter--;
      }
    }
    else if (controllers == 2)
    {
      for (int i = 80, buttoncounter = 0; i <= 260; i += 60, buttoncounter++)
      {
        for (int j = 0; j < 240; j += 60, buttoncounter++)
        {

          if ((p.x < j + 60 && p.x > j) && (p.y < i + 60 && p.y > i))
          {
            if (lengthXrYrZrXYZTh[buttoncounter / 2] > 0 && buttoncounter % 2 == 0)
            {
              //tft.drawLine(lengthXrYrZrXYZTh[buttoncounter / 2]  + (buttoncounter / 2 % 2 * 119), i + 1, lengthXrYrZrXYZTh[buttoncounter / 2] + (buttoncounter / 2 % 2 * 119), i + 58, B);
             if (buttoncounter/2==7)
              {
                showCredits();
              }
              else {
                tft.setCursor(j + 36, i + 20);
                tft.fillRect(j + 32 , i + 1, 30, 58, B);
                tft.fillRect(j + 40, i + 1, 30, 58, B);
                tft.fillRect(j + 62, i + 1, 30, 58, B);
                lengthXrYrZrXYZTh[buttoncounter / 2]--;
                tft.println(lengthXrYrZrXYZTh[buttoncounter / 2] );
              }
            }
            else if (lengthXrYrZrXYZTh[buttoncounter / 2] < 100 && buttoncounter % 2 == 1)
            {
              //tft.drawLine(lengthXrYrZrXYZTh[buttoncounter / 2] + 1 + (buttoncounter / 2 % 2 * 119), i + 1, lengthXrYrZrXYZTh[buttoncounter / 2] + (buttoncounter / 2 % 2 * 119) + 1, i + 58, G);
            if (buttoncounter/2==7)
              {
                showCredits();
              }
              else
              {
                tft.setCursor(j - 24, i + 20);
                tft.fillRect(j + 2 , i + 1, 30, 58, B);
                tft.fillRect(j - 8, i + 1, 30, 58, B);
                tft.fillRect(j - 28, i + 1, 30, 58, B);
                lengthXrYrZrXYZTh[buttoncounter / 2]++;
                tft.println(lengthXrYrZrXYZTh[buttoncounter / 2] );
              }
            }


          }


        } buttoncounter--;

        Joystick.setRxAxis(lengthXrYrZrXYZTh[0]);
        Joystick.setRyAxis(lengthXrYrZrXYZTh[1]);
        Joystick.setRzAxis(lengthXrYrZrXYZTh[2]);
        Joystick.setXAxis(lengthXrYrZrXYZTh[3]);
        Joystick.setYAxis(lengthXrYrZrXYZTh[4]);
        Joystick.setZAxis(lengthXrYrZrXYZTh[5]);
        Joystick.setThrottle(lengthXrYrZrXYZTh[6]);

      }
      /* for (int i = 80, buttoncounter = 32; i <= 260; i += 60, buttoncounter += 2)
        {
        for (int j = 0, counterleftright = 0; j <= 120; j += 120, buttoncounter += 2, counterleftright++)
        {
         tft.setCursor(j + 24, i + 20);
         tft.println(buttonstext[buttoncounter] );
         tft.setCursor(j + 84, i + 20);
         tft.println(buttonstext[buttoncounter + 1] );
        }
        buttoncounter -= 2;
        }*/
    }
  }
  else if (controllers == 0 || controllers == 1)
  {
    for (int i = 80, buttoncounter = controllers * 16; i <= 260; i += 60, buttoncounter++)
    {
      for (int j = 0; j < 240; j += 60, buttoncounter++)
      {
        if (buttons[buttoncounter] == 1)
        {
          tft.fillRect(j + 1, i + 1, 58, 58, R);
          tft.setCursor(j + 14, i + 20);
          tft.println(buttonstext[buttoncounter] );
          Joystick.setButton(buttoncounter, 0);
          buttons[buttoncounter] = 0;
        }
      } buttoncounter--;
    }
  }
}
void displayButtons(int range)
{ tft.fillScreen(B);
  tft.setTextSize(8);
  for (int i = 0; i <= 160; i += 80)
  {
    tft.fillRect(i, 4, 78, 76, R);
    tft.setCursor(i + 20, 12);
    tft.println(charji[i / 80]);
  }
  controllers = range / 16;
  tft.setTextSize(3);
  if (controllers == 0 || controllers == 1)
  {
    for (int i = 80, buttoncounter = range; i <= 260; i += 60, buttoncounter++)
    {
      for (int j = 0; j < 240; j += 60, buttoncounter++)
      {
        tft.fillRect(j + 1, i + 1, 58, 58, R);
        tft.setCursor(j + 14, i + 20);
        tft.println(buttonstext[buttoncounter] );
        buttons[buttoncounter] = 0;
        Joystick.setButton(buttoncounter, 0);
      }
      buttoncounter--;
    }
  }
  else if (controllers == 2)
  {
    for (int i = 80, buttoncounter = range; i <= 260; i += 60, buttoncounter += 2)
    {
      for (int j = 0, counterleftright = 0; j <= 120; j += 120, buttoncounter += 2, counterleftright++)
      {
        tft.drawRect(j - counterleftright , i, 119, 60, R);
        tft.setCursor(j + 14, i + 20);
        tft.println(buttonstext[buttoncounter] );
        tft.setCursor(j + 36, i + 20);
        if (i == 260 && j == 120)
        {
        }
        else
        {
          tft.println(lengthXrYrZrXYZTh[(buttoncounter - range) / 2] );
        }
        tft.setCursor(j + 94, i + 20);
        tft.println(buttonstext[buttoncounter + 1] );
        buttons[buttoncounter] = 0;
        Joystick.setButton(buttoncounter, 0);
      }
      buttoncounter -= 2;
    }
  }
}
void showCredits()
{
   controllers=3;
  tft.fillScreen(B);
  tft.setTextSize(8);
  for (int i = 0; i <= 160; i += 80)
  {
    tft.fillRect(i, 4, 78, 76, R);
    tft.setCursor(i + 20, 12);
    tft.println(charji[i / 80]);
  }
  tft.setTextSize(2);
  tft.setCursor(0, 100);
  tft.println("This porject was created and is maintained by Bostjan Cegovnik aka bokiloki.");
  tft.setCursor(0, 200);
  tft.println("Any suggestions and bugs can be reported in the github repository.");
}
