#include <Arduino.h>
#include "myoledmenu.cpp"
#include <SPI.h>
#include <Wire.h>
#include <Keypad.h>
#include <HID-Project.h>
#include <TimerOne.h>
#include "myencoder.cpp"
#include "mymenuler.cpp"
#include "mykeypad.cpp"
#include <Fonts/FreeSansOblique9pt7b.h>

byte menuId = 1;
byte encoderGorev = 0;
byte mouseGorev = 0;
char customKey = ' ';
//#define KEY_F6				0xC7
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

ClickEncoder *encoder; // variable representing the rotary encoder
int16_t last, value;   // variables for current and last rotation value

byte rowPins[ROWS] = {7, 6, 5};
byte colPins[COLS] = {8, 9, 10, 11};

const char hexaKeys[ROWS][COLS] = {
    {'S', 'E', '-', '-'},
    {'1', '2', '3', '4'},
    {'5', '6', '7', '8'}};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
int sonXisaretci = 1;
int sonYisaretci = -1;

const byte xEksen[5] = {0, 32, 56, 80, 104};

void timerIsr()
{
  encoder->service();
}
void etrafindaKareOlustur(int x, int y)
{
  if (x >= 0 && y >= 0)
  {
    sonXisaretci = x;
    sonYisaretci = y;

    for (int i = 0; i < 4; i++)
    {
      display.drawRect(xEksen[i], y, 24, 24, 0);
    }
    display.drawRect(x, y, 24, 24, 1);
    display.display();
  }
}
void oledTextGoster(int x, int y, String mesaj)
{
  // display.clearDisplay();
  display.setTextSize(1);
  display.setFont(&FreeSansOblique9pt7b);
  display.setTextColor(WHITE);
  display.setCursor(x, y);
  display.println(mesaj);
  display.display();
  delay(20);
}
void testdrawchar(byte menuId)
{
  display.clearDisplay();
  switch (menuId)
  {
  case 0: // Page-1

    display.drawLine(30, 0, 30, 64, WHITE);   // dik ayırma çizgisi
    display.drawLine(30, 32, 128, 32, WHITE); // yatay ayırma çizgisi
    encoderGorev == 0 ? display.drawBitmap(xEksen[0] - 4, 0, MouseScrollIcon, 32, 32, 1) : encoderGorev == 1 ? display.drawBitmap(xEksen[0], 0, MouseLeftRightIcon, 24, 24, 1)
                                                                                                             : display.drawBitmap(xEksen[0], 0, MouseUpDownIcon, 24, 24, 1);
    display.drawBitmap(xEksen[0] - 2, 32, MouseIcon, 28, 28, 1);
    display.drawBitmap(xEksen[1], 0, OrbitIcon, 24, 24, 1);
    display.drawBitmap(xEksen[2], 0, HandIcon, 24, 24, 1);
    display.drawBitmap(xEksen[3], 0, ZoomFitIcon, 24, 24, 1);
    oledTextGoster(xEksen[1] + 2, 60, "Fusion 360");

    break;
  case 1: // Page-2

    // display.drawLine(0, 0, 30, 64, WHITE);   // dik ayırma çizgisi
    display.drawLine(0, 32, 128, 32, WHITE); // yatay ayırma çizgisi
    display.drawBitmap(xEksen[0], 0, P2_Sahne1Icon, 24, 24, 1);
    display.drawBitmap(xEksen[1], 0, P2_Sahne2Icon, 24, 24, 1);
    // display.drawBitmap(xEksen[2], 0, HandIcon, 24, 24, 1);
    // display.drawBitmap(xEksen[3], 0, ZoomFitIcon, 24, 24, 1);
    oledTextGoster(xEksen[1] + 2, 60, "Obx Studio");

    break;
  }

  display.display();
  delay(20);
}

void setup()
{
  Serial.begin(9600); // Opens the serial connection used for communication with the PC.
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  Consumer.begin();
  Keyboard.begin();
  encoder = new ClickEncoder(ENCODER_DT, ENCODER_CLK, ENCODER_SW); // Initializes the rotary encoder with the mentioned pins
  Timer1.initialize(1000);                                         // Initializes the timer, which the rotary encoder uses to detect rotation
  Timer1.attachInterrupt(timerIsr);
  last = -1;
  testdrawchar(menuId);
}
void klavyemouseSerbestBirak()
{
  Keyboard.releaseAll();
  Mouse.releaseAll();

  delay(40);
}

void loop()
{

  customKey = customKeypad.getKey();
  if (customKey)
  {
    // oledTextGoster(customKey);
    // Serial.println(customKey);
    switch (menuId)
    {

    case 0: // Fusion 360 Page-1
      switch (customKey)
      {
      case 'S': // SCROLL
        mouseGorev++;
        encoderGorev = (mouseGorev % 3);
        testdrawchar(menuId);
        if (sonXisaretci >= 0 || sonYisaretci >= 0)
          etrafindaKareOlustur(sonXisaretci, sonYisaretci);
        break;

      case 'E': // escape
        display.drawRect(sonXisaretci, sonYisaretci, 24, 24, 0);
        display.display();
        sonXisaretci = -1;
        sonYisaretci = -1;
        klavyemouseSerbestBirak();
        Keyboard.press(KEY_ESC);
        break;
      case '1': // x eksen
        klavyemouseSerbestBirak();
        etrafindaKareOlustur(xEksen[1], 0);
        Keyboard.press(KEY_LEFT_SHIFT);
        Mouse.press(MOUSE_MIDDLE);
        delay(40);
        break;
      case '2':
        klavyemouseSerbestBirak();
        etrafindaKareOlustur(xEksen[2], 0);
        Mouse.press(MOUSE_MIDDLE);
        delay(40);
        break;
      case '3': // e0 08
        klavyemouseSerbestBirak();
        etrafindaKareOlustur(xEksen[3], 0);
        Keyboard.press(KEY_F6);
        delay(40);
        Keyboard.release(KEY_F6);
        display.drawRect(xEksen[3], 0, 24, 24, 0);
        display.display();
        sonXisaretci = -1;
        sonYisaretci = -1;
        break;
      case '4': // e008
        Consumer.write(MEDIA_VOLUME_MUTE);
        break;

      case '9': //
        // Serial.println('<');
        Keyboard.press('<');
        delay(40);
        Keyboard.releaseAll();
        break;
      }
      break;
    case 1: // Obx Studio Page-2
    switch (customKey)
      {
    
      case 'E': // escape
        display.drawRect(sonXisaretci, sonYisaretci, 24, 24, 0);
        display.display();
        sonXisaretci = -1;
        sonYisaretci = -1;
        klavyemouseSerbestBirak();
        Keyboard.press(KEY_ESC);
        break;
      case '1': // x eksen
        klavyemouseSerbestBirak();
        etrafindaKareOlustur(xEksen[0], 0);
        Keyboard.press(KEY_F13);
        delay(40);
        break;
      case '2':
        klavyemouseSerbestBirak();
        etrafindaKareOlustur(xEksen[1], 0);
        Keyboard.press(KEY_F14);
        delay(40);
        break;}
   
    break;
    }
  }
  value += encoder->getValue();

  // This part of the code is responsible for the actions when you rotate the encoder
  if (value != last)
  { // New value is different than the last one, that means to encoder was rotated
    switch (menuId)
    {
    case 0: // Page-1
      if (last < value)
      {
        Serial.print(F("encoderGorev:"));
        Serial.println(encoderGorev);
        // Consumer.write(MEDIA_VOLUME_UP);
        encoderGorev == 0 ? Mouse.move(0, 0, -1) : encoderGorev == 1 ? Mouse.move(10, 0, 0)
                                                                     : Mouse.move(0, 10, 0);
      } // Detecting the direction of rotation
        // Replace this line to have a different function when rotating counter-clockwise
      else
      {
        // Consumer.write(MEDIA_VOLUME_DOWN);
        encoderGorev == 0 ? Mouse.move(0, 0, 1) : encoderGorev == 1 ? Mouse.move(-10, 0, 0)
                                                                    : Mouse.move(0, -10, 0);
      }
      break;
    case 1: // Page-2
      if (last < value)
      {
        // Keyboard.press(KEY_LEFT_ARROW);
        Mouse.move(10, 0, 0);
        // Keyboard.releaseAll();
      }
      else
      {
        // Keyboard.press(KEY_RIGHT_ARROW);
        // Keyboard.releaseAll();
        Mouse.move(-10, 0, 0);
      }
      break;
    }

    // Replace this line to have a different function when rotating clockwise
    last = value; // Refreshing the "last" varible for the next loop with the current value
    // Serial.print("Encoder Value: "); // Text output of the rotation value used manily for debugging (open Tools - Serial Monitor to see it)
    // Serial.println(value);
  }

  // This next part handles the rotary encoder BUTTON
  ClickEncoder::Button b = encoder->getButton(); // Asking the button for it's current state
  if (b != ClickEncoder::Open)
  {
    if (menuId < 1)
    {
      menuId++;
    }
    else
      menuId = 0;
    // Serial.print("Menu: ");
    // Serial.println(menuId); 
    sonXisaretci=-1;sonYisaretci=-1;
    testdrawchar(menuId);
   
  }

  delay(10); // Wait 10 milliseconds, we definitely don't need to detect the rotary encoder any faster than that
  // The end of the loop() function, it will start again from the beggining (the begginging of the loop function, not the whole document)
}
