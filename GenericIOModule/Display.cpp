
#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#include "Display.h"
#include "Log.h"

#define CHAR_WIDTH_PIXELS 6
#define CHAR_HEIGHT_PIXELS 10

Display::Display(
  int oledClk, int oledMosi, int oledResetPin, int oledDcPin, int oledCsPin, 
  int oledWidth, int oledHeight
) : 
  _oled(Adafruit_SH1107(oledWidth, oledHeight, oledMosi, oledClk, oledDcPin, oledResetPin, oledCsPin)),
  _rows(oledHeight / CHAR_HEIGHT_PIXELS),
  _cols(oledWidth / CHAR_WIDTH_PIXELS)
{}

void Display::_pos(int col, int row) {
  int pixelCol = col < 0 ? (_cols + col) * CHAR_WIDTH_PIXELS : col * CHAR_WIDTH_PIXELS;
  int pixelRow = row < 0 ? (_rows + row) * CHAR_HEIGHT_PIXELS : row * CHAR_HEIGHT_PIXELS;
  __logDebug__("Display:_pos(col,row)=", col, ",", row, ",pCol=", pixelCol, ",pRow=", pixelRow);
  _oled.setCursor(pixelCol, pixelRow);
}

void Display::_cfgText(bool inverse) {
  __logDebug__("Display:_cfgText,inverse=", inverse);
  _oled.setTextSize(1);
  _oled.setTextColor(inverse ? SH110X_BLACK : SH110X_WHITE, inverse ? SH110X_WHITE : SH110X_BLACK); // black background remove residue text
}

void Display::printText(int col, int row, const String &text, bool inverse) {
  __logDebug__("Display:printText(col,row)=", col, ",", row, ",val=", text, ",inv=", inverse);
  _cfgText(inverse);
  _pos(col, row);
  _oled.print(text);
}

void Display::printText(int col, int row, const __FlashStringHelper *ifsh, bool inverse) {
  __logDebug__("Display:printText(col,row)=", col, ",", row, ",val=", ifsh, ",inv=", inverse);
  _cfgText(inverse);
  _pos(col, row);
  _oled.print(ifsh);
}

void Display::printChar(int col, int row, char ch, bool inverse) {
  __logDebug__("Display:printChar(col,row)=", col, ",", row, ",val=", ch, ",inv=", inverse);
  _cfgText(inverse);
  _pos(col, row);
  _oled.write(ch);
}


void Display::attach() {

  __logDebug__("Display:attach - start");

  if (!_oled.begin()) {
    __logError__("Display:attach - Failed to start SH1107 OLED");
    delay(100);
  }
  delay(100);
  _oled.clearDisplay();
  _oled.display();
  __logDebug__("Display cleared, maxCol=", _cols, ",maxRows=", _rows);

  __logDebug__("Display:attach - completed");
}

void Display::listen() {
  // do nothing
}

void Display::update(bool clear) {
  __logDebug__("Display:update - start, clear=", clear);
  if (clear) {
    _oled.clearDisplay();
  }
  _oled.display();
  __logDebug__("Display:update - completed");
}

