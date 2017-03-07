#include <Adafruit_TFTLCD.h> //Library
#include <DS1307RTC.h>
#include <Adafruit_GFX_AS.h>
#include <font/FreeSans9pt7b.h>
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

// Agar warna mudah dimengerti (Human Readable color):
#define TFT_BLACK   0x0000
#define TFT_BLUE    0x001F
#define TFT_RED     0xF800
#define TFT_GREEN   0x07E0
#define TFT_CYAN    0x07FF
#define TFT_MAGENTA 0xF81F
#define TFT_YELLOW  0xFFE0
#define TFT_WHITE   0xFFFF
#define TFT_GREY    0x5AEB

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // Saved H, M, S x & y coords
int16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;
uint32_t targetTime = 0;                    // for next 1 second timeout
char d;
uint16_t xpos; // x posisi jam

int hh = 0;
int mm = 0;
int ss = 0;
long previousMillis = 0;
int interval = 1000;

//Mengambil waktu jam dari waktu Compile/Upload di system Windows
//uint8_t hh = conv2d(__TIME__), mm = conv2d(__TIME__ + 3), ss = conv2d(__TIME__ + 6); // mengambil data waktu dari jam Compile-Upload
boolean initial = 1;

void setup(void) {
  tft.reset();        //perlu saat menggunakan lib.SPFD5408_Adafruit_TFTLCD.h
  tft.begin(0x9341);  //perlu address ini saat menggunakan lib.SPFD5408_Adafruit_TFTLCD.h
  tft.setRotation(0); //Lanscape Mode

  tft.setTextColor(TFT_WHITE);  // warna font
  tft.fillScreen(TFT_BLACK);    //warna latar

  // Draw clock face
  xpos = tft.width() / 2; // mencari titik koordinat tengah LCD
  tft.fillCircle(xpos, 120, 118, TFT_BLUE); //warna lingkaran luar
  tft.fillCircle(xpos, 120, 110, TFT_BLACK); //warna jam bagian dalam

  // Draw 12 lines
  for (int i = 0; i < 360; i += 30) {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    x0 = sx * 114 + xpos;
    yy0 = sy * 114 + 120;
    x1 = sx * 100 + xpos;
    yy1 = sy * 100 + 120;

    tft.drawLine(x0, yy0, x1, yy1, TFT_YELLOW);//garis penanda angka jam
  }

  // Draw 60 dots
  for (int i = 0; i < 360; i += 6) {
    sx = cos((i - 90) * 0.0174532925);
    sy = sin((i - 90) * 0.0174532925);
    x0 = sx * 102 + xpos;
    yy0 = sy * 102 + 120;
    // Draw minute markers
    tft.drawPixel(x0, yy0, TFT_WHITE); //titik penanda menit
    // Draw main quadrant dots
    if (i == 0 || i == 180) tft.fillCircle(x0, yy0, 3, TFT_WHITE); //penanda 12 dan 6
    if (i == 90 || i == 270) tft.fillCircle(x0, yy0, 3, TFT_WHITE); //penanda 3 dan 9
  }

  tft.fillCircle(xpos, 121, 3, TFT_WHITE);
  targetTime = millis() + 1000;

}

void loop() {
  tmElements_t tm;
  RTC.read(tm);
  hh = tm.Hour;
  mm = tm.Minute;
  ss = tm.Second;
  //   tft.fillRect(5, 260,240,20,TFT_BLACK);
  if (targetTime < millis()) {
    targetTime = millis() + 1000;
    ss++;              // Advance second
    if (ss == 60) {
      ss = 0;
      mm++;            // Advance minute
      if (mm > 59) {
        mm = 0;
        hh++;          // Advance hour
        if (hh > 23) {
          hh = 0;
        }
      }
    }

    // Pre-compute hand degrees, x & y coords for a fast screen update
    sdeg = ss * 6;                     // 0-59 -> 0-354
    mdeg = mm * 6 + sdeg * 0.01666667; // 0-59 -> 0-360 - includes seconds, but these increments are not used
    hdeg = hh * 30 + mdeg * 0.0833333; // 0-11 -> 0-360 - includes minutes and seconds, but these increments are not used
    hx = cos((hdeg - 90) * 0.0174532925);
    hy = sin((hdeg - 90) * 0.0174532925);
    mx = cos((mdeg - 90) * 0.0174532925);
    my = sin((mdeg - 90) * 0.0174532925);
    sx = cos((sdeg - 90) * 0.0174532925);
    sy = sin((sdeg - 90) * 0.0174532925);

    if (ss == 0 || initial) {
      initial = 0;
      // Erase hour and minute hand positions every minute
      tft.drawLine(ohx, ohy, xpos, 121, TFT_BLACK);
      ohx = hx * 62 + xpos + 1;
      ohy = hy * 62 + 121;
      tft.drawLine(omx, omy, xpos, 121, TFT_BLACK);
      omx = mx * 84 + xpos;
      omy = my * 84 + 121;
    }

    // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
    tft.drawLine(osx, osy, xpos, 121, TFT_BLACK);
    osx = sx * 90 + xpos + 1;
    osy = sy * 90 + 121;
    tft.drawLine(osx, osy, xpos, 121, TFT_RED);
    tft.drawLine(ohx, ohy, xpos, 121, TFT_WHITE);
    tft.drawLine(omx, omy, xpos, 121, TFT_WHITE);
    tft.drawLine(osx, osy, xpos, 121, TFT_RED);

    tft.fillCircle(xpos, 121, 3, TFT_RED);
    tft.drawCircle(120, 203, 10, TFT_BLUE);
    // Draw text at position xpos,260 using fonts 4
    // Only font numbers 2,4,6,7 are valid. Font 6 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : . a p m
    // Font 7 is a 7 segment font and only contains characters [space] 0 1 2 3 4 5 6 7 8 9 : .
    // tft.fillRect(xpos-5, 200,12,10,TFT_BLACK);
    tft.setCursor(xpos - 5, 200);
    tft.setTextSize(1);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    if (ss < 10) {
      tft.print("0"); tft.print(ss);
    } else
      tft.print(ss);
  }
  if (hh >= 0 && hh < 12) d = 'A'; else {
    d = 'P';
  }
  tft.drawRoundRect(50, 110, 29, 21, 5, TFT_BLUE);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(53, 113);
  tft.setTextSize(2);
  tft.print(d);
  tft.println('M');


  tft.setTextSize(2);
  tft.setCursor(8, 280);
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.print("DATE :");
  tft.setCursor(100, 280);
  if (tm.Day < 10) {
    tft.print("0");
  }
  tft.print(tm.Day);
  tft.print(".");
  if (tm.Month < 10) {
    tft.print("0");
  }
  tft.print(tm.Month);
  tft.print(".");
  tft.print(tmYearToCalendar(tm.Year));

  tft.setCursor(8, 260);
  tft.print("TIME :");
  tft.setCursor(100, 260);
  if (tm.Hour < 10) {
    tft.print("0");
  }
  tft.print(tm.Hour);
  tft.print(":");
  if (tm.Minute < 10) {
    tft.print("0");
  }
  tft.print((tm.Minute));
  tft.print(":");
  if (tm.Second < 10) {
    tft.print("0");
  }
  tft.print((tm.Second));
  tft.setFont(&FreeSans9pt7b);
  tft.setCursor(0, 315);
  tft.setTextSize(1);
  tft.setTextColor(TFT_RED);
  tft.print("Emanuele Principi 2017");
  tft.setFont();
  tft.setCursor(92, 55);
  tft.setTextSize(2);
  tft.setTextColor(TFT_GREEN);
  tft.print("ROLEX");
  tft.setTextColor(TFT_WHITE);
}


