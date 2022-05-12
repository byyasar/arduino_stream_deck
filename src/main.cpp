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

byte menuId = 0;
byte buton = 0;
char customKey = ' ';
//#define KEY_F6				0xC7
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

ClickEncoder *encoder; // variable representing the rotary encoder
int16_t last, value;   // variables for current and last rotation value

byte rowPins[ROWS] = {7, 6, 5};
byte colPins[COLS] = {8, 9, 10, 11};

const char hexaKeys[ROWS][COLS] = {
    {'+', '-', '-', '-'},
    {'1', '2', '3', '4'},
    {'5', '6', '7', '8'}};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void timerIsr()
{
  encoder->service();
}

void testdrawchar(byte menuId)
{
  display.clearDisplay();
  switch (menuId)
  {
  case 0:
    display.drawBitmap(0, 0, menuoledillustrator, 128, 64, 1);

    display.display();
    break;
  case 1:
    // display.drawBitmap(0, 0, menu_kodlama, 128, 64, 1);
    break;
  case 2:
    // display.drawBitmap(0, 0, windowslogo, 128, 64, 1);
    break;
  case 3:
    // display.drawBitmap(0, 0, fusion360, 128, 64, 1);
    break;
  }

  display.display();
  delay(20);
}
void oledTextGoster(char basilanTus)
{
  String tusGorev = "-Bulunamadı -";
  int id = basilanTus - 48;
  switch (menuId)
  {
  case 0:

    tusGorev = menuOBX[id - 1];
  }
  /*
  void Adafruit_GFX::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                                int16_t w, int16_t h, uint16_t color) {

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    startWrite();
    for (int16_t j = 0; j < h; j++, y++) {
      for (int16_t i = 0; i < w; i++) {
        if (i & 7)
          byte <<= 1;
        else
          byte = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
        if (byte & 0x80)
          writePixel(x + i, y, color);
      }
    }
    endWrite();
  }


  */

  display.clearDisplay();
  for (size_t i = 0; i < 32; i++)
  {
    for (size_t j = 0; j < 32; j++)
    {
      Serial.println(menuoledillustrator[(i * j) + j]);

      if (menuoledillustrator[j * ((32 + 7) / 8) + i / 8] & (1 << (i % 8)))
      {
        display.drawPixel(i, j, 1);
      }
      else
      {
        display.drawPixel(i, j, 0);
      }
    }
  }

  // display.drawBitmap(0, 0, menuoledillustrator, 24, 24, 1);

  display.setTextSize(1);              // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(65, 30);           // Start at top-left corner

  display.println(tusGorev);
  display.display();
  delay(1000);
  testdrawchar(menuId);
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

void loop()
{

  customKey = customKeypad.getKey();
  if (customKey)
  {
    oledTextGoster(customKey);
    Serial.println(customKey);
    switch (menuId)
    {

    case 0: // windows kod
      switch (customKey)
      {
      case '+': // shift+left-geri

        Keyboard.releaseAll();
        Mouse.releaseAll();
        break;
      case '1':

        Keyboard.press(KEY_LEFT_SHIFT); // Command key in Mac, use KEY_LEFT_CTRL for PcCommand key
        Mouse.press(MOUSE_MIDDLE);

        delay(40);
        break;
      case '2':
        Mouse.press(MOUSE_MIDDLE);
        delay(40);
        Keyboard.releaseAll();
        break;
      case '3': // e0 08
        Keyboard.press(KEY_F6);
        delay(40);
        Keyboard.release(KEY_F6);
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
    case 1: // uygulama kod
      break;
    case 2: // sistem kod
      break;
    case 3: // sistem kod
      break;

    default:
      break;
    }
  }
  value += encoder->getValue();

  // This part of the code is responsible for the actions when you rotate the encoder
  if (value != last)
  { // New value is different than the last one, that means to encoder was rotated
    switch (menuId)
    {
    case 0:
      if (last < value)
      {
        //Consumer.write(MEDIA_VOLUME_UP);
        Mouse.move( 0,0,2);
      } // Detecting the direction of rotation
        // Replace this line to have a different function when rotating counter-clockwise
      else
      {
        //Consumer.write(MEDIA_VOLUME_DOWN);
        Mouse.move( 0,0,-2);
      }
      break;
    case 1:
      if (last < value)
      {
        //Keyboard.press(KEY_LEFT_ARROW);
        Mouse.move( -10,0,0);
        //Keyboard.releaseAll();
      }
      else
      {
        // Keyboard.press(KEY_RIGHT_ARROW);
        // Keyboard.releaseAll();
         Mouse.move( 10,0,0);
      }
      break;
      case 2:
      if (last < value)
      {
        //Keyboard.press(KEY_LEFT_ARROW);
        Mouse.move( 0,-10,0);
        //Keyboard.releaseAll();
      }
      else
      {
        // Keyboard.press(KEY_RIGHT_ARROW);
        // Keyboard.releaseAll();
         Mouse.move( 0,10,0);
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
    if (menuId < 3)
    {
      menuId++;
    }
    else
      menuId = 0;
    // Serial.print("Menu: ");
    // Serial.println(menuId);
    testdrawchar(menuId);
  }

  delay(10); // Wait 10 milliseconds, we definitely don't need to detect the rotary encoder any faster than that
  // The end of the loop() function, it will start again from the beggining (the begginging of the loop function, not the whole document)
}
