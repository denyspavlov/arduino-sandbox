#ifndef __DISPLAY__
#define __DISPLAY__

#include <Arduino.h>
#include <Adafruit_SH110X.h>

/**
 * Display object
 */
class Display {
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
   */
  Display(
    int oledClk, int oledMosi, int oledResetPin, int oledDcPin, int oledCsPin, 
    int oledWidth, int oledHeight
  );

  /**
   * Hardware initialization. Use in setup().
   */
  void attach();

  /**
   * Use in loop()
   */
  void listen();

  /**
   * Send updates to hardware.
   *
   * clear - clear display buffer (blanck screen)
   */
  void update(bool clear = false);

  /**
   * Print text at specific location.
   *
   * col     - character position (not pixel)
   * row     - charcater row (not pixel)
   * text    - text to print
   * inverse - false = white on black, true = black on white 
   */
  void printText(int col, int row, const String &text, bool inverse);

  /**
   * Print text at specific location.
   *
   * col     - character position (not pixel)
   * row     - charcater row (not pixel)
   * text    - text to print
   * inverse - false = white on black, true = black on white 
   */
  void printText(int col, int row, const __FlashStringHelper *ifsh, bool inverse);

  /**
   * Print char at specific location.
   *
   * col     - character position (not pixel)
   * row     - charcater row (not pixel)
   * ch      - character to print
   * inverse - false = white on black, true = black on white 
   */
  void printChar(int col, int row, char ch, bool inverse);

private:
  Adafruit_SH1107 _oled;
  const int _rows;
  const int _cols;
  void _cfgText(bool);
  void _pos(int, int);
};

#endif