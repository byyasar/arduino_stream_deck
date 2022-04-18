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
char customKey = ' ';

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

ClickEncoder *encoder; // variable representing the rotary encoder
int16_t last, value;   // variables for current and last rotation value

 byte rowPins[ROWS] = {9, 8, 7, 6};
 byte colPins[COLS] = {5, 4, A4, A5};

const char hexaKeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

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
    display.drawBitmap(0, 0, menu_kodlama, 128, 64, 1);
    break;
  case 2:
    display.drawBitmap(0, 0, windowslogo, 128, 64, 1);
    break;
  case 3:
    display.drawBitmap(0, 0, fusion360, 128, 64, 1);
    break;
  }

  display.display();
  delay(20);
}
void oledTextGoster(char basilanTus)
{
  String tusGorev = "-Bulunamadı -";
int id =basilanTus - 48;
  switch (menuId)
  {
  case 0:
  
    tusGorev=menuOBX[id-1];
  }
   
display.clearDisplay();
display.setTextSize(1);              // Normal 1:1 pixel scale
display.setTextColor(SSD1306_WHITE); // Draw white text
display.setCursor(0, 0);             // Start at top-left corner
display.print("Tus Gorevi :");
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
    // Initializes the media keyboard
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
      case '0':                       // shift+left-geri
        Keyboard.press(KEY_LEFT_GUI); // Command key in Mac, use KEY_LEFT_CTRL for PcCommand key
        Keyboard.write(KEY_TAB);
        delay(150);
        Keyboard.releaseAll();
        /*delay(100);
        Keyboard.write(0x20) ;
        delay(40);
        Keyboard.write(KEY_BACKSPACE);
        delay(40); //
        Keyboard.releaseAll();*/
        break;
      case '1':
        Keyboard.write(KEY_LEFT_SHIFT);
        Keyboard.write(KEY_LEFT_ARROW);
        delay(40);
        Keyboard.releaseAll();
        break;
      case '2':
        Keyboard.write(KEY_LEFT_SHIFT);
        Keyboard.write(KEY_RIGHT_ARROW);
        delay(40);
        Keyboard.releaseAll();
        break;
      case '3': // e0 08
        Keyboard.press(0x20);
        delay(40);
        Keyboard.release(0x20);
        break;
      case '4': // e008
        Consumer.write(MEDIA_VOLUME_MUTE);
        break;
      case '9': //
        Serial.println('<');
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
        Consumer.write(MEDIA_VOLUME_UP);
      } // Detecting the direction of rotation
        // Replace this line to have a different function when rotating counter-clockwise
      else
      {
        Consumer.write(MEDIA_VOLUME_DOWN);
      }
      break;
    case 1:
      if (last < value)
      {
        Keyboard.press(KEY_LEFT_ARROW);
        Keyboard.releaseAll();
      }
      else
      {
        Keyboard.press(KEY_RIGHT_ARROW);
        Keyboard.releaseAll();
      }
      break;
    }

    // Replace this line to have a different function when rotating clockwise
    last = value;                    // Refreshing the "last" varible for the next loop with the current value
    Serial.print("Encoder Value: "); // Text output of the rotation value used manily for debugging (open Tools - Serial Monitor to see it)
    Serial.println(value);
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
    Serial.print("Menu: ");
    Serial.println(menuId);
    testdrawchar(menuId);

    // If the button is unpressed, we'll skip to the end of this if block
    // Serial.print("Button: ");
    //#define VERBOSECASE(label) case label: Serial.println(#label); break;
    //  switch (b) {
    //    case ClickEncoder::Clicked: // Button was clicked once
    //      Consumer.write(MEDIA_PLAY_PAUSE); // Replace this line to have a different function when clicking button once
    //      break;

    //   case ClickEncoder::DoubleClicked: // Button was double clicked
    //     Consumer.write(MEDIA_NEXT); // Replace this line to have a different function when double-clicking
    //     break;
    // }
  }

  delay(10); // Wait 10 milliseconds, we definitely don't need to detect the rotary encoder any faster than that
  // The end of the loop() function, it will start again from the beggining (the begginging of the loop function, not the whole document)
}

/*
    This is just a long comment
    Here are some fun functions you can use to replace the default behaviour
    Consumer.write(CONSUMER_BRIGHTNESS_UP);
    Consumer.write(CONSUMER_BRIGHTNESS_DOWN);
    Consumer.write(CONSUMER_BROWSER_HOME);
    Consumer.write(CONSUMER_SCREENSAVER);
    Consumer.write(HID_CONSUMER_AL_CALCULATOR); //launch calculator :)
    Consumer.write(HID_CONSUMER_AC_ZOOM_IN);
    Consumer.write(HID_CONSUMER_AC_SCROLL_UP);
    CONSUMER_SLEEP = 0x32,

    FULL LIST CAN BE FOUND HERE:
    https://github.com/NicoHood/HID/blob/master/src/HID-APIs/ConsumerAPI.h
*/
