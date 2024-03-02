#ifndef __LOG_H__
#define __LOG_H__

#include <Arduino.h> 

// Logging library using preprocessor macros. 
// Basic usage is to declare logging level and then include the h file at the top of main script
// ...
// #define LOG_LEVEL 3
// #include "Log.h"
// ...
//
// LOG_LEVEL values to control logging output
// 0 OFF
// 1 ERROR only
// 2 ERROR and INFO only
// 3 ERROR, INFO and DEBUG
// ~ OFF

#if !defined(LOG_LEVEL)
  #define LOG_LEVEL 0 // Default log level
#endif

#define DEBUG_PREFIX "DEBUG: "
#define INFO_PREFIX  "INFO : "
#define ERROR_PREFIX "ERROR: "
#define LOG_COMMA    ","

#if LOG_LEVEL == 3

#define _debugSP1(a) { \
    Serial.print(F(DEBUG_PREFIX)); \
    Serial.println(a); \
}

#define _debugSP2(a, b) { \
    Serial.print(F(DEBUG_PREFIX)); \
    Serial.print(a); \
    Serial.println(b); \
}

#define _debugSP3(a, b, c) { \
    Serial.print(F(DEBUG_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(LOG_COMMA)); \
    Serial.println(c); \
}

#define _debugSP4(a, b, c, d) { \
    Serial.print(F(DEBUG_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.println(d); \
}

#define _debugSP5(a, b, c, d, e) { \
    Serial.print(F(DEBUG_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.print(d); \
    Serial.print(F(LOG_COMMA)); \
    Serial.println(e); \
}

#define _debugSP6(a, b, c, d, e, f) { \
    Serial.print(F(DEBUG_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.print(d); \
    Serial.print(F(e)); \
    Serial.println(f); \
}

#define _debugSP7(a, b, c, d, e, f, g) { \
    Serial.print(F(DEBUG_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.print(d); \
    Serial.print(F(e)); \
    Serial.print(f); \
    Serial.print(F(LOG_COMMA)); \
    Serial.println(g); \
}

#define _debugSP8(a, b, c, d, e, f, g, h) { \
    Serial.print(F(DEBUG_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.print(d); \
    Serial.print(F(e)); \
    Serial.print(f); \
    Serial.print(F(g)); \
    Serial.println(h); \
}

#endif // DEBUG fuctions end

#if LOG_LEVEL > 1 && LOG_LEVEL <= 3

#define _infoSP1(a) { \
    Serial.print(F(INFO_PREFIX)); \
    Serial.println(a); \
}

#define _infoSP2(a, b) { \
    Serial.print(F(INFO_PREFIX)); \
    Serial.print(a); \
    Serial.println(b); \
}

#define _infoSP3(a, b, c) { \
    Serial.print(F(INFO_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(LOG_COMMA)); \
    Serial.println(c); \
}

#define _infoSP4(a, b, c, d) { \
    Serial.print(F(INFO_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.println(d); \
}

#define _infoSP5(a, b, c, d, e) { \
    Serial.print(F(INFO_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.print(d); \
    Serial.print(F(LOG_COMMA)); \
    Serial.println(e); \
}

#define _infoSP6(a, b, c, d, e, f) { \
    Serial.print(F(INFO_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.print(d); \
    Serial.print(F(e)); \
    Serial.println(f); \
}

#define _infoSP7(a, b, c, d, e, f, g) { \
    Serial.print(F(INFO_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.print(d); \
    Serial.print(F(e)); \
    Serial.print(f); \
    Serial.print(F(LOG_COMMA)); \
    Serial.println(g); \
}

#define _infoSP8(a, b, c, d, e, f, g, h) { \
    Serial.print(F(INFO_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.print(d); \
    Serial.print(F(e)); \
    Serial.print(f); \
    Serial.print(F(g)); \
    Serial.println(h); \
}

#endif // INFO functions end

#if LOG_LEVEL > 0 && LOG_LEVEL <= 3

#define _errorSP1(a) { \
    Serial.print(F(ERROR_PREFIX)); \
    Serial.println(a); \
}

#define _errorSP2(a, b) { \
    Serial.print(F(ERROR_PREFIX)); \
    Serial.print(a); \
    Serial.println(b); \
}

#define _errorSP3(a, b, c) { \
    Serial.print(F(ERROR_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(LOG_COMMA)); \
    Serial.println(c); \
}

#define _errorSP4(a, b, c, d) { \
    Serial.print(F(ERROR_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.println(d); \
}

#define _errorSP5(a, b, c, d, e) { \
    Serial.print(F(ERROR_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.print(d); \
    Serial.print(F(LOG_COMMA)); \
    Serial.println(e); \
}

#define _errorSP6(a, b, c, d, e, f) { \
    Serial.print(F(ERROR_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.print(d); \
    Serial.print(F(e)); \
    Serial.println(f); \
}

#define _errorSP7(a, b, c, d, e, f, g) { \
    Serial.print(F(ERROR_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.print(d); \
    Serial.print(F(e)); \
    Serial.print(f); \
    Serial.print(F(LOG_COMMA)); \
    Serial.println(g); \
}

#define _errorSP8(a, b, c, d, e, f, g, h) { \
    Serial.print(F(ERROR_PREFIX)); \
    Serial.print(F(a)); \
    Serial.print(b); \
    Serial.print(F(c)); \
    Serial.print(d); \
    Serial.print(F(e)); \
    Serial.print(f); \
    Serial.print(F(g)); \
    Serial.println(h); \
}

#endif // ERROR functions end

// Macro selector based on parametes
#define GET_SP_FUNC(_1, _2, _3, _4, _5, _6, _7, _8, NAME, ...) NAME

#if LOG_LEVEL == 0
  #define __logDebug__(...) 
  #define __logInfo__(...) 
  #define __logError__(...) 
#elif LOG_LEVEL == 1
  #define __logDebug__(...) 
  #define __logInfo__(...) 
  #define __logError__(...) GET_SP_FUNC(__VA_ARGS__, _errorSP8, _errorSP7, _errorSP6, _errorSP5, _errorSP4, _errorSP3, _errorSP2, _errorSP1)(__VA_ARGS__) 
#elif LOG_LEVEL == 2
  #define __logDebug__(...) 
  #define __logInfo__(...) GET_SP_FUNC(__VA_ARGS__, _infoSP8, _infoSP7, _infoSP6, _infoSP5, _infoSP4, _infoSP3, _infoSP2, _infoSP1)(__VA_ARGS__) 
  #define __logError__(...) GET_SP_FUNC(__VA_ARGS__, _errorSP8, _errorSP7, _errorSP6, _errorSP5, _errorSP4, _errorSP3, _errorSP2, _errorSP1)(__VA_ARGS__) 
#elif LOG_LEVEL == 3
  #define __logDebug__(...) GET_SP_FUNC(__VA_ARGS__, _debugSP8, _debugSP7, _debugSP6, _debugSP5, _debugSP4, _debugSP3, _debugSP2, _debugSP1)(__VA_ARGS__) 
  #define __logInfo__(...) GET_SP_FUNC(__VA_ARGS__, _infoSP8, _infoSP7, _infoSP6, _infoSP5, _infoSP4, _infoSP3, _infoSP2, _infoSP1)(__VA_ARGS__) 
  #define __logError__(...) GET_SP_FUNC(__VA_ARGS__, _errorSP8, _errorSP7, _errorSP6, _errorSP5, _errorSP4, _errorSP3, _errorSP2, _errorSP1)(__VA_ARGS__) 
#else
  #define __logDebug__(...) 
  #define __logInfo__(...) 
  #define __logError__(...) 
#endif

#endif