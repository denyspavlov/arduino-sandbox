#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>

#include <WiFi.h>

// debug
#define LOG_LEVEL 3
#include "Log.h"

// custom components
#include "Display.h"
#include "Keyboard.h"
#include "PushButtonArray.h"
#include "Alarm.h"

#define INPUT_SENSITIVITY 100

// OLED SH1107 config over SPI

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

#define OLED_SPI_CLK    18
#define OLED_SPI_MOSI   19
#define OLED_SPI_RESET   5
#define OLED_DC         26
#define OLED_CS         33


Display display(
  OLED_SPI_CLK, OLED_SPI_MOSI, OLED_SPI_RESET, OLED_DC, OLED_CS, 
  SCREEN_WIDTH, SCREEN_HEIGHT
);

// 4x button array on single pin

#define BTN_PIN 34
#define BTN_MODE 0
#define BTN_CLR 1
#define BTN_SEL 2
#define BTN_DEL 3

PushButtonArray btns(
  BTN_PIN, 4, new int[8] { 1100, 1500, 1700, 2000, 600, 900, 0, 200 }, HIGH, INPUT_SENSITIVITY
); 

// Potentiomener for selection

#define PTN_PIN 35
#define PTN_MIN 4094
#define PTN_MAX 0

RangeSelector sel(
  PTN_PIN, INPUT_SENSITIVITY, 1, 1, PTN_MIN, PTN_MAX
);

// Keyboard (mediator) that uses, display, 4x button array and potentiometer

#define KBD_LAYOUT_FULL "ABCDEFGHJIKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()-+_=:;\"|\/<>{}[],.               "
#define KBD_LAYOUT_FULL_COLS 21
#define KBD_LAYOUT_FULL_ROWS 5

#define KBD_LAYOUT_NUM  "0123456789           "
#define KBD_LAYOUT_NUM_COLS 21
#define KBD_LAYOUT_NUM_ROWS 1

Keyboard kbd(
  &display,
  &sel,
  &btns, BTN_MODE, BTN_DEL, BTN_SEL, BTN_CLR
);

#define ALR_PIN  17
#define ALR_INRV 300
#define ALR_MAX  1500

Alarm alr(
  ALR_PIN, ALR_INRV
);

// WiFi

#define MAX_SCAN_TIME 5


// System state control
bool inputMode = false;

bool wifiConnecting = true;
int ssidi = -1;
char * ssid;
char * pw;



void onInputModeListener() {
  __logDebug__("input mode ON");
}

void onCaptureListener(char* captured, int len) {

  __logDebug__("onCaptureListener=", captured, ",size=", len);

  if (wifiConnecting) {

    if (ssidi == -1) {
      ssidi = atoi(captured);
      String name = WiFi.SSID(ssidi);
      ssid = static_cast<char*>(malloc(sizeof(char) * (name.length() + 1))); 
      name.toCharArray(ssid, name.length() + 1);
      __logDebug__("onCaptureListener,ssidi=", ssidi, ",ssid=", ssid);
    } else {
      pw = static_cast<char*>(malloc(sizeof(char) * (len + 1)));
      memcpy(pw, captured, len + 1);
      __logDebug__("onCaptureListener,pw=", pw);
    }

  } else {

    display.printText(0, 0, F("Your input was:"), false);
    display.printText(0, 1, captured, false);
    display.update();

  }

  inputMode = false;

}

void onButtonPushListener(int btn) {

  __logDebug__("onButtonPushListener=", btn);

  if (!wifiConnecting) {

    if (btn == BTN_MODE) {
      if (!inputMode) {
        inputMode = true;
        kbd.readInput("Testing Keyboard.\nType something.", KBD_LAYOUT_FULL, KBD_LAYOUT_FULL_COLS, KBD_LAYOUT_FULL_ROWS);
      }
    }

  }

}

void onSelectorListener(int x, int y) {


}

void onAlarmChange(bool toggle) {

}

bool connectToWiFi() {
  
  __logDebug__("connectToWiFi - start");  

  display.update(true);
  display.printText(0, 0, F("Initiating WiFi ..."), false);
  display.update();
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(200);

  display.update(true);
  display.printText(0, 0, F("Scanning WiFi ..."), false);
  display.update();

  int n = WiFi.scanNetworks();
  
  __logDebug__("connectToWiFi,n=", n);  

  if (n < 1) {
    display.printText(0, 1, F("no networks found."), false);
    display.update();
    __logDebug__("connectToWiFi - finish - FAIL (no networks)");
    return false;
  } else {
    String prompt = "Choose network:";
    for (int i = 0; i < n; i++) {
      if (WiFi.RSSI(i) >= -70) { // only good reception
        prompt += "\n" + String(i) + ") " + WiFi.SSID(i);
        __logDebug__("connectToWiFi,i=", i, ",ssid=", WiFi.SSID(i));  
      }
    }

    char tempPrompt[100];
    prompt.toCharArray(tempPrompt, prompt.length() + 1);

    kbd.readInput(tempPrompt, KBD_LAYOUT_NUM, KBD_LAYOUT_NUM_COLS, KBD_LAYOUT_NUM_ROWS);
    inputMode = true;
    while (inputMode) {
      kbd.listen();
    }

    String nt = "Enter password for:\n" + WiFi.SSID(ssidi);
    nt.toCharArray(tempPrompt, nt.length() + 1);

    kbd.readInput(tempPrompt, KBD_LAYOUT_FULL, KBD_LAYOUT_FULL_COLS, KBD_LAYOUT_FULL_ROWS);
    inputMode = true;
    while (inputMode) {
      kbd.listen();
    }

    __logDebug__("connectToWiFi - connecting"); 
    String cn = "Connecting to:\n" + WiFi.SSID(ssidi);
    cn.toCharArray(tempPrompt, cn.length() + 1);
    display.printText(0, 0, cn, false);
    display.update();

    WiFi.mode(WIFI_STA);
    //WiFi.begin(ssid, pw);
    WiFi.begin("VM0916253", "Lg3sgckdzfjv");


    bool blink = 0;
    int count = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      display.printChar(0, 2, ' ', blink);
      display.update();
      blink ^= 1;
      count++;
      __logDebug__("connectToWiFi - connecting, ssid=", ssid, ",pw=", pw , ",status=", WiFi.status()); 
      if (count > 20) {
        __logDebug__("connectToWiFi - finish - FAIL (unable to connect)");
        ssidi = -1; // reset selection to enter re-select
        display.printText(0, 2, F("Unable to connect (check network & password)"), false);
        display.update();
        while(true) {
          if (BTN_MODE == btns.listen()) {
            break;
          }
        }
        return false;
      }
    }

    display.printText(0, 2, F("Connected:"), false);
    display.printText(0, 3, WiFi.localIP().toString(), false);

    display.update();

    WiFi.scanDelete();

    delay(100);

  }
  
  __logDebug__("connectToWiFi - finish - OK");
  wifiConnecting = false;

  alr.toggle(true, ALR_MAX);

  return true;

}

void setup() {
  Serial.begin(115200);

  __logDebug__("setup - start");

  display.attach();
  display.printText(0, 0, F("Starting..."), false);
  display.update();

  btns.attach(onButtonPushListener);
  sel.attach(onSelectorListener);
  alr.attach(onAlarmChange);

  kbd.attach(onInputModeListener, onCaptureListener);

  display.update(true);

  __logDebug__("setup - finish");

  while(!connectToWiFi()) {
    delay(5000);
  }

}

void loop() {

  if (inputMode) {
    kbd.listen();
  } else {
    btns.listen();
  }

  alr.listen();

  // __logDebug__("PTN:", analogRead(PTN_PIN));
  // __logDebug__("BTN:", analogRead(BTN_PIN));
  // delay(300);
}

