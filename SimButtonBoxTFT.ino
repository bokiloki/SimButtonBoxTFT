#include <Adafruit_GFX.h>
#include <Adafruit_TFTLCD.h>
#include <Joystick.h>
#include <TouchScreen.h>

namespace Config {
constexpr uint16_t displayDriver = 0x9341;
constexpr int16_t touchMinX = 150, touchMaxX = 920;
constexpr int16_t touchMinY = 120, touchMaxY = 940;
constexpr int16_t pressureMin = 10, pressureMax = 1000;
constexpr uint16_t axisRepeatMs = 90;
constexpr uint8_t axisStep = 1;
}

namespace Color {
constexpr uint16_t background = 0xF800;
constexpr uint16_t idle = 0xFBE0;
constexpr uint16_t pressed = 0x07E0;
constexpr uint16_t text = 0x0000;
constexpr uint16_t border = 0xFFFF;
}

constexpr int16_t screenWidth = 240, screenHeight = 320;
constexpr int16_t tabHeight = 80, cellSize = 60;

TouchScreen touchScreen(6, A1, A2, 7, 274);
Adafruit_TFTLCD tft(A3, A2, A1, A0, A4);
Joystick_ joystick;

enum Page : uint8_t { BUTTONS_A, BUTTONS_B, AXES, CREDITS };
Page page = BUTTONS_A;
int8_t activeButton = -1;
int8_t activeAxisControl = -1;
bool touchWasDown = false;
uint32_t lastAxisChange = 0;
// Preserve the original startup behavior: every HID axis begins at zero.
uint8_t axisValues[7] = {0, 0, 0, 0, 0, 0, 0};

const char *const tabs[] = {"A", "B", "C"};
const char *const axisNames[] = {"Rx", "Ry", "Rz", "X", "Y", "Z", "THR"};

void drawPage();
void drawTabs();
void drawButtonPage();
void drawAxesPage();
void drawAxis(uint8_t axis);
void drawCredits();
void scanTouch();
void releaseButton();
void changeAxis(uint8_t axis, int8_t direction);
void sendAxes();

void setup() {
  Serial.begin(115200);
  tft.reset();
  tft.begin(Config::displayDriver);
  tft.setRotation(0);
  tft.setTextColor(Color::text);
  tft.setTextWrap(true);

  joystick.setRxAxisRange(0, 100);
  joystick.setRyAxisRange(0, 100);
  joystick.setRzAxisRange(0, 100);
  joystick.setXAxisRange(0, 100);
  joystick.setYAxisRange(0, 100);
  joystick.setZAxisRange(0, 100);
  joystick.setThrottleRange(0, 100);
  joystick.begin();
  sendAxes();
  drawPage();
}

void loop() { scanTouch(); }

void scanTouch() {
  TSPoint point = touchScreen.getPoint();
  // Display and touch share these pins; restore them after each touch read.
  pinMode(A2, OUTPUT);
  pinMode(A1, OUTPUT);

  const bool down = point.z > Config::pressureMin && point.z < Config::pressureMax;
  if (!down) {
    releaseButton();
    activeAxisControl = -1;
    touchWasDown = false;
    return;
  }

  if (touchWasDown) {
    if (page == AXES && activeAxisControl >= 0 &&
        millis() - lastAxisChange >= Config::axisRepeatMs) {
      changeAxis(activeAxisControl / 2, activeAxisControl % 2 ? 1 : -1);
      lastAxisChange = millis();
    }
    return;
  }

  touchWasDown = true;
  int16_t x = screenWidth - map(point.x, Config::touchMinX, Config::touchMaxX,
                                screenWidth, 0);
  int16_t y = screenHeight - map(point.y, Config::touchMinY, Config::touchMaxY,
                                 screenHeight, 0);
  x = constrain(x, 0, screenWidth - 1);
  y = constrain(y, 0, screenHeight - 1);

  if (y < tabHeight) {
    releaseButton();
    page = static_cast<Page>(x / 80);
    drawPage();
    return;
  }

  const uint8_t row = (y - tabHeight) / cellSize;
  if (row >= 4) return;

  if (page == BUTTONS_A || page == BUTTONS_B) {
    const uint8_t column = x / cellSize;
    const uint8_t offset = page == BUTTONS_B ? 16 : 0;
    activeButton = offset + row * 4 + column;
    joystick.setButton(activeButton, 1);
    tft.fillRect(column * cellSize + 1, tabHeight + row * cellSize + 1,
                 58, 58, Color::pressed);
    tft.setTextSize(3);
    tft.setCursor(column * cellSize + 13, tabHeight + row * cellSize + 20);
    if (activeButton + 1 < 10) tft.print('0');
    tft.print(activeButton + 1);
  } else if (page == AXES) {
    const uint8_t control = row * 2 + x / 120;
    if (control == 7) {
      page = CREDITS;
      drawPage();
      return;
    }
    const bool plus = x % 120 >= 60;
    activeAxisControl = control * 2 + (plus ? 1 : 0);
    changeAxis(control, plus ? 1 : -1);
    lastAxisChange = millis();
  }
}

void releaseButton() {
  if (activeButton < 0) return;
  joystick.setButton(activeButton, 0);
  activeButton = -1;
  if (page == BUTTONS_A || page == BUTTONS_B) drawButtonPage();
}

void changeAxis(uint8_t axis, int8_t direction) {
  if (axis >= 7) return;
  axisValues[axis] = constrain(
      static_cast<int16_t>(axisValues[axis]) + direction * Config::axisStep,
      0, 100);
  sendAxes();
  drawAxis(axis);
}

void sendAxes() {
  joystick.setRxAxis(axisValues[0]);
  joystick.setRyAxis(axisValues[1]);
  joystick.setRzAxis(axisValues[2]);
  joystick.setXAxis(axisValues[3]);
  joystick.setYAxis(axisValues[4]);
  joystick.setZAxis(axisValues[5]);
  joystick.setThrottle(axisValues[6]);
}

void drawPage() {
  tft.fillScreen(Color::background);
  drawTabs();
  if (page == BUTTONS_A || page == BUTTONS_B) drawButtonPage();
  else if (page == AXES) drawAxesPage();
  else drawCredits();
}

void drawTabs() {
  tft.setTextSize(8);
  for (uint8_t i = 0; i < 3; ++i) {
    tft.fillRect(i * 80, 4, 78, 76, page == i ? Color::pressed : Color::idle);
    tft.setCursor(i * 80 + 20, 12);
    tft.print(tabs[i]);
  }
}

void drawButtonPage() {
  const uint8_t offset = page == BUTTONS_B ? 16 : 0;
  tft.setTextSize(3);
  for (uint8_t row = 0; row < 4; ++row) {
    for (uint8_t column = 0; column < 4; ++column) {
      const uint8_t button = offset + row * 4 + column;
      const int16_t x = column * cellSize, y = tabHeight + row * cellSize;
      tft.fillRect(x + 1, y + 1, 58, 58, Color::idle);
      tft.setCursor(x + 13, y + 20);
      if (button + 1 < 10) tft.print('0');
      tft.print(button + 1);
    }
  }
}

void drawAxesPage() {
  for (uint8_t axis = 0; axis < 7; ++axis) drawAxis(axis);
  tft.drawRect(119, 259, 120, 60, Color::border);
  tft.setTextSize(2);
  tft.setCursor(139, 280);
  tft.print("ABOUT");
}

void drawAxis(uint8_t axis) {
  const int16_t x = (axis % 2) * 120;
  const int16_t y = tabHeight + (axis / 2) * cellSize;
  tft.fillRect(x, y, 120, 60, Color::background);
  tft.drawRect(x, y, 120, 60, Color::border);
  tft.setTextSize(2);
  tft.setCursor(x + 4, y + 4);
  tft.print(axisNames[axis]);
  tft.setTextSize(3);
  tft.setCursor(x + 8, y + 27);
  tft.print('-');
  tft.setCursor(x + 42, y + 27);
  if (axisValues[axis] < 100) tft.print(' ');
  if (axisValues[axis] < 10) tft.print(' ');
  tft.print(axisValues[axis]);
  tft.setCursor(x + 96, y + 27);
  tft.print('+');
}

void drawCredits() {
  tft.setTextSize(2);
  tft.setCursor(8, 100);
  tft.println("SimButtonBoxTFT");
  tft.println();
  tft.println("Created and maintained by");
  tft.println("Bostjan Cegovnik (bokiloki).");
  tft.println();
  tft.println("Suggestions and bug reports:");
  tft.println("github.com/bokiloki/");
  tft.println("SimButtonBoxTFT");
}
