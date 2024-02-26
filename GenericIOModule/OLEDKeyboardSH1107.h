#ifndef __OLED_KEYBOARD_SH1107__
#define __OLED_KEYBOARD_SH1107__

#include <Arduino.h>
#include <Adafruit_SH110X.h>
#include "PushButton.h"
#include "RangeSelector.h"

#define MAX_INPUT 20

/**
 * Potentiometer selector that can remap to a matrix
 */
class OLEDKeyboardSH1107 {
public:
  /**
   * Constructor that initialises the object (not hardware).
   * Recommended to setup in global.
   * 
   * OLED (SPI):
   * clk          - SPI clock
   * MOSI         - SPI MOSI
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
  OLEDKeyboardSH1107(
    int oledClk, int oledMosi, int oledResetPin, int oledDcPin, int oledCsPin, int oledWidth, int oledHeight,
    int ptnPin, int ptnLow, int ptnHigh,
    int btnPin, int btnInputA1, int btnInputA2, int btnClearA1, int btnClearA2, int btnSelectA1, int btnSelectA2, int btnDeleteA1, int btnDeleteA2, 
    int btnIdleState = HIGH, unsigned long sensitivity = 100);
  
  /**
   * Hardware initialization. Use in setup().
   * 
   * onInputModeListener  - callback function that will trigger when we eneter input mode
   * onCaptureListener    - callback function that will trigger once we have a non zero length input
   */
  void attach(void (*onInputModeListener)(void), void (*onCaptureListener)(byte*,int));

  /**
   * Takes reading and saves toggle value
   */
  void listen();

  /**
   * Enter the input mode with given input layout.
   * 
   * prompt - prompt to the user which requires an input
   * layout - byte array of input characters available to select
   * rangeX - number of options in a row
   * rangeY - number of rows
   */
  void readInput(char prompt[], byte * layout, int rangeX, int rangeY);

private:
  Adafruit_SH1107 _oled;
  RangeSelector _rsl;
  PushButton _mode;
  PushButton _clr;
  PushButton _sel;
  PushButton _del;
  int _cursorX = 0;
  int _cursorY = 0;
  int _rangeX = 1;
  int _rangeY = 1;
  bool _modeToggle = 1;
  int _inputLength = 0;
  byte _input[MAX_INPUT + 1];
  char * _prompt;
  int _promptLength = 0;
  byte * _layout;
  void (*_onInputModeListener)(void);
  void (*_onCaptureListener)(byte*,int);

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