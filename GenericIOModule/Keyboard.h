#ifndef __KEYBOARD__
#define __KEYBOARD__

#include <Arduino.h>
#include "Display.h"
#include "PushButtonArray.h"
#include "RangeSelector.h"

#define MAX_INPUT 20

/**
 * Potentiometer selector that can remap to a matrix
 */
class Keyboard {
public:
  /**
   * Constructor that initialises the object (not hardware).
   * Recommended to setup in global.
   * 
   * OLED:
   * display      - shared display object
   * Potentiometer:
   * rsl          - share potentiometer/ select wheel (analog)
   * Control buttons (x4)
   * btns         - shared button array (analog)
   * btnMode      - mode button index in the array
   * btnDel       - delete character button index in the array
   * btnSel       - select character button index in the array
   * btnClr       - clear input button index in the array
   */
  Keyboard(
    Display * display,
    RangeSelector * rsl,
    PushButtonArray * btns, int btnMode = 0, int btnDel = 1, int btnSel = 2, int btnClr = 3);
  
  /**
   * Hardware initialization. Use in setup().
   * 
   * onInputModeListener  - callback function that will trigger when we eneter input mode
   * onCaptureListener    - callback function that will trigger once we have a non zero length input
   */
  void attach(void (*onInputModeListener)(void), void (*onCaptureListener)(char*,int));

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
  void readInput(char prompt[], char layout[], int rangeX, int rangeY);

private:
  
  Display * _display;
  
  RangeSelector * _rsl;
  
  PushButtonArray * _btns;
  int _btnMode = 0; 
  int _btnDel = 1; 
  int _btnSel = 2; 
  int _btnClr = 3;
  
  int _cursorX = 0;
  int _cursorY = 0;
  int _rangeX = 1;
  int _rangeY = 1;
  bool _inputMode = 0;
  int _inputLength = 0;
  char * _input = new char[MAX_INPUT + 1];
  char * _layout;
  void (*_onInputModeListener)(void);
  void (*_onCaptureListener)(char*,int);

  void _onMode();
  void _onClr();
  void _onSel();
  void _onDel();
  void _onRsl(int x, int y);
  void _displayPrintLayout(int selectX, int selectY);
  void _displayPrintLayoutChangeCursor(int oldX, int oldY, int selectX, int selectY);
  void _displayClearLayout();
  void _displayPrintInputValue();
  
};

#endif