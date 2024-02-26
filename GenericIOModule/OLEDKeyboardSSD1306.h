#ifndef __OLED_KEYBOARD_SSD1306__
#define __OLED_KEYBOARD_SSD1306__

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "PushButton.h"
#include "RangeSelector.h"

#define MAX_INPUT 20

/**
 * Potentiometer selector that can remap to a matrix
 */
class OLEDKeyboardSSD1306 {
public:
  /**
   * Constructor that initialises the object (not hardware).
   * Recommended to setup in global.
   * 
   * OLED (SPI):
   * oledResetPin - reset pin (e.g. 8)
   * oledDcPin    - DC pin (e.g. 6)
   * oledCsPin    - CS pin (e.g. 7)
   * Potentiometer:
   * ptnPin       - Potentiometer pin (analog e.g. A5)
   * ptnLow       - lowest reading
   * ptnHigh      - highest reading
   * Control buttons (x4)
   * btnInputPin  - Input mode push button (digital)
   * btnClearPin  - Clear input push button (digital)
   * btnSelectPin - Select character push button (digital)
   * btnDeletePin - Delete character button (digital)
   * btnIdleState - idle (not pressed) state for button (LOW/HIGH)
   * General:
   * sensitivity  - timeout between reads
   */
  OLEDKeyboardSSD1306(
    int oledResetPin, int oledDcPin, int oledCsPin, int oledWidth, int oledHeight,
    int ptnPin, int ptnLow, int ptnHigh,
    int btnInputPin, int btnClearPin, int btnSelectPin, int btnDeletePin, 
    int btnIdleState = HIGH, unsigned long sensitivity = 100);
  
  /**
   * Hardware initialization. Use in setup().
   * 
   * onInputModeListener  - callback function that will trigger when we eneter input mode
   * onCaptureListener    - callback function that will trigger once we have a non zero length input
   */
  void attach(byte* (*onInputModeListener)(void), void (*onCaptureListener)(byte*));

  /**
   * Takes reading and saves toggle value
   */
  void listen();

  /**
   * Enter the input mode with given input layout.
   * 
   * layout - byte array of input characters available to select
   */
  void readInput(byte * layout);

private:
  Adafruit_SSD1306 _oled;
  RangeSelector _rsl;
  PushButton _mode;
  PushButton _clr;
  PushButton _sel;
  PushButton _del;
  int _cursorX = 0;
  int _cursorY = 0;
  int _rangeX = 0;
  int _rangeY = 0;
  bool _modeToggle = 1;
  int _inputLength = 0;
  byte _input[MAX_INPUT + 1] = { '\0' };
  byte * _layout;
  byte* (*_onInputModeListener)(void);
  void (*_onCaptureListener)(byte*);

  void _onMode();
  void _onClr();
  void _onSel();
  void _onDel();
  void _onRsl(int x, int y);
  void _oledPrint(int col, int row, String text, bool inverse);
  void _oledPrintWelcome();
  void _oledPrintInput(int selectX, int selectY);
  void _oledPrintInputChangeCursor(int oldX, int oldY, int selectX, int selectY);
  void _oledPrintInputValue();
  
};

#endif