 /******************************
*  OneKey Morse BLE Keyboard sample
*  Copyright(c) @tomorroa56 all rights reserved
*  
*  Special thanks to:
*  <M5Unified>
*  https://github.com/m5stack/M5Unified 
*  <CW Decorder>
*  https://monoist.itmedia.co.jp/mn/articles/2211/24/news020_2.html
*  https://github.com/imaoca/OneKey
*  <Morse Code>
*  https://en.wikipedia.org/wiki/Morse_code
*  <NimBLE Keyboard>
*  https://github.com/wakwak-koba/ESP32-NimBLE-Keyboard
******************************/

#if defined ( ARDUINO )
  #include <Arduino.h>
#endif

#include <M5Unified.h>
#include <esp_log.h>

// for BLE Keyboard
#include <BleKeyboard.h>
BleKeyboard bleKeyboard("CW-Keyboard", "M5Stack", 100);

// for Morse Decode
unsigned long signal_len,t1,t2,t0;  //time for which button is pressed
String code = "";                   //string in which one alphabet is stored
const int spacePeriod = 2000;       //Waiting period until a space is entered
const int debouncingPeriod = 20;    //switch debouncing
const int longpressLimit = 500;     //Limit of long Press time
boolean connectDotDash = true;     //Code connect flag
int inputPin = G37;                 //M5StickC BtnA for input

void setup(void)
{
  auto cfg = M5.config();

#if defined ( ARDUINO )
  cfg.serial_baudrate = 115200;   // default=115200. if "Serial" is not needed, set it to 0.
#endif
  cfg.clear_display = true;  // default=true. clear the screen when begin.
  cfg.output_power  = false;  // default=true. use external port 5V output.
  cfg.internal_imu  = false;  // default=true. use internal IMU.
  cfg.internal_rtc  = false;  // default=true. use internal RTC.
  cfg.internal_spk  = false;  // default=true. use internal speaker.
  cfg.internal_mic  = false;  // default=true. use internal microphone.

  M5.begin(cfg);

  M5.Display.setBrightness(64);

  if (M5.Display.width() < M5.Display.height())
  { /// Landscape mode.
    M5.Display.setRotation(M5.Display.getRotation() ^ 1);
  }

  int textsize = M5.Display.height() / 160;
  if (textsize == 0)
  {
    textsize = 1;
  }
  M5.Display.setTextSize(textsize);

  // hardware model check
  char* boardName = boardType();
  M5.Display.startWrite();
  M5.Display.print("Core:");
  M5.Display.println(boardName);
  ESP_LOGI("setup", "core:%s", boardName);
  M5.Display.println("Morse Code Keyboard");

  // Key setting internal pullup
  pinMode(inputPin, INPUT_PULLUP);

  // start BLE Keyboard
  Serial.println("Starting BLE work!");
  bleKeyboard.begin();
}

void loop(void)
{
  vTaskDelay(1);
  int h = M5.Display.height() / 8;
  int w = M5.Display.width() / 12;

//  M5.update();

  M5.Display.setCursor(w*5, h*3);
  M5.Display.setTextSize(3);

  if(bleKeyboard.isConnected())
  {
    M5.Display.fillRect(w*11, h/2, w, h, TFT_GREEN);
    while (connectDotDash == true)
    {
      t0 = millis();
      // waiting for button press
      while (digitalRead(inputPin) == HIGH)
      {
      }
      // Enter a space if no key is pressed for a certain period of time
      if (millis()-t0 > spacePeriod)
      {
        bleKeyboard.print(' ');
        Serial.print(' ');
      }
      t1 = millis();                            //time at button press
      // waiting for button released
      while (digitalRead(inputPin) == LOW)
      {
      }
      t2 = millis();                            //time at button release
      signal_len = t2 - t1;                     //time for which button is pressed
      if (signal_len > debouncingPeriod)        //to account for switch debouncing
      {
        code += readio();                       //function to read dot or dash
      }
      //limit of long press
      //if time between button press greater than 0.5sec, skip loop and go to next alphabet
      while ((millis() - t2) < longpressLimit)
      {     
        if (digitalRead(inputPin) == LOW)
        {
          connectDotDash = true;
          break;
        }else
        {
          connectDotDash = false;
        }
      }
    }
    char keyin = convertor();       //function to decipher code into alphabet
    Serial.print(keyin);
    M5.Display.print(keyin);
    bleKeyboard.print(keyin);
    connectDotDash = true;
  }
  else
  {
    M5.Display.fillRect(w*11, h/2, w, h, TFT_RED);
  }
}

/**********
* hardware model check
**********/
char* boardType()
{
  char* name;
  switch (M5.getBoard())
  {
#if defined (CONFIG_IDF_TARGET_ESP32C3)
  case m5::board_t::board_M5StampC3:
    name = "StampC3";
    break;
  case m5::board_t::board_M5StampC3U:
    name = "StampC3U";
    break;
#else
  case m5::board_t::board_M5Stack:
    name = "Stack";
    break;
  case m5::board_t::board_M5StackCore2:
    name = "StackCore2";
    break;
  case m5::board_t::board_M5StickC:
    name = "StickC";
    break;
  case m5::board_t::board_M5StickCPlus:
    name = "StickCPlus";
    break;
  case m5::board_t::board_M5StackCoreInk:
    name = "CoreInk";
    break;
  case m5::board_t::board_M5Paper:
    name = "Paper";
    break;
  case m5::board_t::board_M5Tough:
    name = "Tough";
    break;
  case m5::board_t::board_M5Station:
    name = "Station";
    break;
  case m5::board_t::board_M5Atom:
    name = "ATOM";
    break;
  case m5::board_t::board_M5AtomPsram:
    name = "ATOM PSRAM";
    break;
  case m5::board_t::board_M5AtomU:
    name = "ATOM U";
    break;
  case m5::board_t::board_M5TimerCam:
    name = "TimerCamera";
    break;
  case m5::board_t::board_M5StampPico:
    name = "StampPico";
    break;
#endif
  default:
    name = "Who am I ?";
    break;
  }

  return name;
}

#if !defined ( ARDUINO )
extern "C" {
  void loopTask(void*)
  {
    setup();
    for (;;) {
      loop();
    }
    vTaskDelete(NULL);
  }

  void app_main()
  {
    xTaskCreatePinnedToCore(loopTask, "loopTask", 8192, NULL, 1, NULL, 1);
  }
}
#endif


/**********
* Morse Decode
**********/
char readio()
{
  if (signal_len < 200 && signal_len > 20)
  {
    return '.';                        //if button press less than 0.2sec, it is a dot
  }
  else //if (signal_len >= 300)
  {
    return '-';                        //if button press more than 0.2sec, it is a dash
  }
}

char convertor()
{
  static String letters[] = {
    ".-",   // A (1)
    "-...", // B
    "-.-.", // C
    "-..",  // D
    ".",    // E
    "..-.", // F
    "--.",  // G
    "....", // H
    "..",   // I
    ".---", // J
    "-.-",  // K
    ".-..", // L
    "--",   // M
    "-.",   // N
    "---",  // O
    ".--.", // P
    "--.-", // Q
    ".-.",  // R
    "...",  // S
    "-",    // T
    "..-",  // U
    "...-", // V
    ".--",  // W
    "-..-", // X
    "-.--", // Y
    "--..", // Z (26)
    "-----",  // 0
    ".----",  // 1
    "..---",  // 2
    "...--",  // 3
    "....-",  // 4
    ".....",  // 5
    "-....",  // 6
    "--...",  // 7
    "---..",  // 8
    "----.",  // 9 (36)
    "E"       // end code
  };

  int i = 0; 
  char c;
  if (code == ".-.-.-")
  {
    c = '.';        //period
  }
  else if (code == "--..--")
  {
    c = ',';        //comma
  }
  else if (code == "..--..")
  {
    c = '?';        //question
  }
  else if (code == ".----.")
  {
    c = 0x27;        //Apostrophe (')
  }
  else if (code == "-.-.--")
  {
    c = '!';        //Exclamation
  }
  else if (code == "-..-.")
  {
    c = '/';        //slash
  }
  else if (code == "-.--.")
  {
    c = '(';        //Open parenthesis
  }
  else if (code == "-.--.-")
  {
    c = ')';        //Close parenthesis
  }
    else if (code == ".-...")
  {
    c = '&';        //ampersand
  }
  else if (code == "---...")
  {
    c = ':';        //colon
  }
  else if (code == "-.-.-.")
  {
    c = ';';        //semicolon ';'
  }
  else if (code == "-...-")
  {
    c = '=';        //double dash
  }
  else if (code == ".-.-.")
  {
    c = '+';        //plus
  }
  else if (code == "-....-")
  {
    c = '-';        //hyphen
  }
  else if (code == "..--.-")
  {
    c = '_';        //underscore
  }
  else if (code == ".-..-.")
  {
    c = '?';        //Question
  }
  else if (code == "...-..-")
  {
    c = '$';        //Dollar
  }
  else if (code == ".--.-.")
  {
    c = '@';        //at mark
  }
  else if (code == "...-.-")
  {
    c = 0x0a;  //end of work, send "LF"
  }
  else
  {
    while (letters[i] != "E")  //loop for comparing input code with letters array
    {
      if (letters[i] == code)
      {
        if (i>25)
          c = '0'+(i-26);
        else
          c = 'A'+i;
        break;
      }
      i++;
    }
    if (letters[i] == "E")
    {
      c = 0x0a;  //if input code doesn't match any letter, send "LF"
//      Serial.println("");  //if input code doesn't match any letter, error
    }
  }
  code = "";                            //reset code to blank string
  return c;
}
