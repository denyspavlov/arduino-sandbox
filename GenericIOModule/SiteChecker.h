#ifndef __SITE_CHECKER__
#define __SITE_CHECKER__

#include <Arduino.h>

#include "Certs.h"
#include "Display.h"
#include "PushButtonArray.h"
#include "Alarm.h"

struct Site {
  const char* id;
  const char* url;
  const char* cert;
  const char* contentVerify;
  int contentVerifyBytes;
  unsigned long lastCheck;
  bool up;
  unsigned long lastUp;
  bool contentOk;
  unsigned long lastContentOk;
};

/**
 * Potentiometer selector that can remap to a matrix
 */
class SiteChecker {
public:
  /**
   * Constructor that initialises the object (not hardware).
   * Recommended to setup in global.
   * 
   * OLED:
   * display      - shared display object
   * Alarm:
   * alr          - shared buzzer
   * Control buttons (x4)
   * btns         - shared button array (analog)
   * btnMode      - mode button index in the array
   * btnDel       - delete character button index in the array
   * btnSel       - select character button index in the array
   * btnClr       - clear input button index in the array
   */
  SiteChecker(
    Display * display,
    Alarm * alr,
    PushButtonArray * btns, int btnMode = 0, int btnMute = 1, int btnCheck = 2, int btnClr = 3);
  
  /**
   * Hardware initialization. Use in setup().
   * 
   * listener   - callback function that will receive toggleFlag as an input
   */
  void attach(void (*listener)(bool) = nullptr);

  /**
   * Takes reading and saves toggle value
   */
  void listen();

  /**
   * Switch the checker on/of
   *
   * on       - true to turn on, false to turn off
   */
  bool toggle(bool on);

private:
  
  Display * _display;

  Alarm * _alr;
  
  PushButtonArray * _btns;
  int _btnMode = 0; 
  int _btnMute = 1; 
  int _btnCheck = 2; 
  int _btnClr = 3;
  
  bool _on = 0;
  bool _mute = 0;
  unsigned long _interval = 60000;
  int _i = 0;
  unsigned long _iLast = 0;

  void _check();
  void _check(Site* site, int i);
  bool _evaluate(Site* site, int i, int maxDown);
  void _reset();
  void _onMode();
  void _onClr();
  void _onCheck(bool change = false);
  void _onMute(bool change = false);
  void (*_listener)(bool);
  
};

#endif