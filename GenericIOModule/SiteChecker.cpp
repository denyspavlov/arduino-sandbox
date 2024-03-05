#include <Arduino.h>
#include <esp_heap_caps.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

#include "SiteChecker.h"
#include "Display.h"
#include "PushButtonArray.h"
#include "Alarm.h"
#include "Log.h"


#define NUM_SITES 8
#define MAX_DOWN 2
#define ALARM_INTERVAL 2000
#define CHECK_INTERVAL 2000

#define __1_MINUTE   60000
#define __3_MINUTE  180000
#define __5_MINUTE  300000
#define __10_MINUTE 600000

extern Site sites[NUM_SITES] = {
  { "IS",
    "https://www.inspire-software.com",
    letsencryptCACertificate,
    "<body",
    40000,
    0,
    false,
    0,
    false,
    0 },
  { "IS-DOC",
    "https://docs.inspire-software.com/docs/display/YD",
    letsencryptCACertificate,
    "<body",
    55000,
    0,
    false,
    0,
    false,
    0 },
  { "IS-JIRA",
    "https://jira.inspire-software.com/jira/secure/Dashboard.jspa",
    letsencryptCACertificate,
    "<body id=\"jira\"",
    40000,
    0,
    false,
    0,
    false,
    0 },
  { "YC",
    "https://www.yes-cart.org",
    letsencryptCACertificate,
    "<body",
    40000,
    0,
    false,
    0,
    false,
    0 },
  { "YC-DEMO",
    "https://demo.yes-cart.org/category/notebooks/query/F551MA-SX112H",
    letsencryptCACertificate,
    "<div class=\"product-name-holder\">",
    35000,
    0,
    false,
    0,
    false,
    0 },
  { "YCE-DEMO",
    "https://edemo.yes-cart.org/category/notebooks/query/F551MA-SX112H",
    letsencryptCACertificate,
    "<div class=\"product-name-holder\">",
    35000,
    0,
    false,
    0,
    false,
    0 },
  { "MMS",
    "https://www.multimediastore.ch/category/7037",
    letsencryptCACertificate,
    "<div class=\"wp-block product",
    180000,
    0,
    false,
    0,
    false,
    0 },
  { "A1",
    "https://b2bwebshop.a1.net/category/14875",
    a1CACertificate,
    "<div class=\"product",
    225000,
    0,
    false,
    0,
    false,
    0 }
};


SiteChecker::SiteChecker(
  Display* display,
  Alarm* alr,
  PushButtonArray* btns, int btnMode, int btnMute, int btnCheck, int btnClr)
  : _display(display),
    _alr(alr),
    _btns(btns), _btnMode(btnMode), _btnMute(btnMute), _btnCheck(btnCheck), _btnClr(btnClr) {}


void SiteChecker::_check(Site* site, int i) {

  unsigned long now = millis();

  WiFiClientSecure* client = new WiFiClientSecure;
  if (client) {
    client->setCACert(site->cert);

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
      HTTPClient https;

      __logDebug__("SiteChecker:_check,site=", site->id, ",URL=", site->url);

      _display->printText(0, i + 1, site->id, false);
      _display->printText(-6, i + 1, F("   ..."), false);
      _display->update();


      if (https.begin(*client, site->url)) {  // HTTPS

        // start connection and send HTTP header
        int httpCode = https.GET();

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          __logDebug__("SiteChecker:_check,site=", site->id, ",URL=", site->url, ",status=", httpCode);

          // file found at server
          if (httpCode == HTTP_CODE_OK) {

            __logDebug__("SiteChecker:_check,site=", site->id, ",URL=", site->url, ",site=", "UP");

            String payload = https.getString();

            bool contentValid = false;
            if (payload.isEmpty()) {
              int len = https.getSize();
              __logDebug__("SiteChecker:_check,site=", site->id, ",URL=", site->url, ",checkingBytes=", len, ",requiredBytes=", site->contentVerifyBytes);
              contentValid = len >= site->contentVerifyBytes;
            } else {
              contentValid = payload.indexOf(site->contentVerify) >= 0;
            }

            if (contentValid) {

              _display->printText(-6, i + 1, F("up/200"), false);
              _display->update();
              __logDebug__("SiteChecker:_check,site=", site->id, ",URL=", site->url, ",site=", "UP", ",content=", "OK");

              site->up = true;
              site->lastUp = now;
              site->contentOk = true;
              site->lastContentOk = now;
            } else {
              _display->printText(-6, i + 1, F("up/400"), false);
              _display->update();
              __logDebug__("SiteChecker:_check,site=", site->id, ",URL=", site->url, ",site=", "UP", ",content=", "ERROR");

              site->up = true;
              site->lastUp = now;
              site->contentOk = false;
            }
          } else {
            _display->printText(-6, i + 1, F("xx/500"), false);
            _display->update();
            __logDebug__("SiteChecker:_check,site=", site->id, ",URL=", site->url, ",site=", "DOWN");

            site->up = false;
            site->contentOk = false;
          }
        } else {

          _display->printText(-6, i + 1, F("??/500"), false);
          _display->update();
          __logDebug__("SiteChecker:_check,site=", site->id, ",URL=", site->url, ",site=", "???", ",error=", https.errorToString(httpCode).c_str());

          site->up = false;
          site->contentOk = false;
        }

        https.end();
      } else {
        _display->printText(-6, i + 1, F("cc/500"), false);
        _display->update();
        __logDebug__("SiteChecker:_check,site=", site->id, ",URL=", site->url, ",site=", "Unable to connect");
      }

      // End extra scoping block
    }

    delete client;
  } else {
    Serial.println("Unable to create client");
  }

  site->lastCheck = now;
}

bool SiteChecker::_evaluate(Site* site, int i, int maxDown) {
  __logDebug__("SiteChecker:_evaluate - start");
  
  if (site->up && site->contentOk) {
    return false;
  } else if (site->up && !site->contentOk) {
    unsigned long now = millis();
    return (now - site->lastContentOk > maxDown * _interval);
  } else {
    unsigned long now = millis();
    return (now - site->lastUp > maxDown * _interval);
  }

  __logDebug__("SiteChecker:_evaluate - end");
}

void SiteChecker::_reset() {
  __logDebug__("SiteChecker:_reset - start");

  _alr->toggle(false);

  unsigned long now = millis();
  for (int i = 0; i < NUM_SITES; i++) {
    sites[i].lastCheck = now;
    sites[i].lastUp = now;
    sites[i].lastContentOk = now;
  }

  __logDebug__("SiteChecker:_reset - end");
}

void SiteChecker::_onMode() {
  __logDebug__("SiteChecker:_onMode - start");

  toggle(!_on);

  __logDebug__("SiteChecker:_onMode - end");
}

void SiteChecker::_onClr() {
  __logDebug__("SiteChecker:_onClr - start");

  _reset();

  __logDebug__("SiteChecker:_onClr - end");
}


void SiteChecker::_check() {

  unsigned long now = millis();

  if (now - _iLast > CHECK_INTERVAL) {
    __logDebug__("SiteChecker:_check - start,_i=", _i);

    bool exceedsDownTime = false;
    if (now - sites[_i].lastCheck > _interval) {
      __logDebug__("SiteChecker:_check,start checking=", sites[_i].id);
      _check(&sites[_i], _i);
      if (_evaluate(&sites[_i], _i, MAX_DOWN)) {
        exceedsDownTime = true;
      }
      __logDebug__("SiteChecker:_check, finish checking=", sites[_i].id);
      if (exceedsDownTime && !_mute) {
        __logDebug__("SiteChecker:_check, site is down");
        _alr->toggle(true, ALARM_INTERVAL);
      }
    }

    if (_i < NUM_SITES - 1) {
      _i++;
    } else {
      _i = 0;
    }

    _iLast = now;

    __logDebug__("SiteChecker:_onCheck - end,next_i=", _i);
  }

}

void SiteChecker::_onCheck(bool change) {
  __logDebug__("SiteChecker:_onCheck - start");

  if (_interval <= __1_MINUTE) {
    if (change) {
      _interval = __3_MINUTE;
      _display->printText(-3, 0, " 3m", false);
      _display->update();
    } else {
      _display->printText(-3, 0, " 1m", false);
      _display->update();
    }
  } else if (_interval <= __3_MINUTE) {
    if (change) {
      _interval = __5_MINUTE;
      _display->printText(-3, 0, " 5m", false);
      _display->update();
    } else {
      _display->printText(-3, 0, " 3m", false);
      _display->update();
    }
  } else if (_interval <= __5_MINUTE) {
    if (change) {
      _interval = __10_MINUTE;
      _display->printText(-3, 0, "10m", false);
      _display->update();
    } else {
      _display->printText(-3, 0, " 5m", false);
      _display->update();
    }
  } else if (_interval <= __10_MINUTE) {
    if (change) {
      _interval = __1_MINUTE;
      _display->printText(-3, 0, " 1m", false);
      _display->update();
    } else {
      _display->printText(-3, 0, "10m", false);
      _display->update();
    }
  }

  __logDebug__("SiteChecker:_onCheck - end");
}


void SiteChecker::_onMute(bool change) {
  __logDebug__("SiteChecker:_onMute - start");

  if (change) {
    _mute ^= 1;
  }

  __logDebug__("SiteChecker:_onMute,_mute=", _mute);
  if (_mute) {
    _alr->toggle(false);
    _display->printText(-6, 0, " ", false);
    _display->update();
  } else {
    _display->printText(-6, 0, "*", false);
    _display->update();
  }


  __logDebug__("SiteChecker:_onMute - end");
}


void SiteChecker::attach(void (*listener)(bool)) {

  __logDebug__("SiteChecker:attach - start");

  _listener = listener;

  __logDebug__("SiteChecker:attach - end");
}


void SiteChecker::listen() {
  if (_on) {
    int push = _btns->listen();
    if (push >= 0) {
      __logDebug__("Keyboard:listen,push=", push);
      if (push == _btnMode) {
        _onMode();
      }
      if (push == _btnClr) {
        _onClr();
      }
      if (push == _btnCheck) {
        _onCheck(true);
      }
      if (push == _btnMute) {
        _onMute(true);
      }
    }
    _check();
  }
}

bool SiteChecker::toggle(bool on) {

  bool change = _on != on;
  _on = on;
  __logDebug__("SiteChecker:toggle=", on);
  _display->update(true);
  if (_on) {
    _reset();
    _display->printText(0, 0, "Scanning: ON ", false);
    _onCheck(false);
    _onMute(false);
  } else {
    _display->printText(0, 0, "Scanning: OFF ", false);
    _onCheck(false);
    _onMute(false);
  }
  _display->update();
  if (change && _listener != nullptr) {
    _listener(_on);
  }
  return change;
}
