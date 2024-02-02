#ifndef __MEM_PROBE__
#define __MEM_PROBE__

#include <stdint.h>

// Built in memory pointers
extern uint8_t _end;
extern uint8_t __stack;
extern uint8_t *__brkval;
extern uint8_t *__data_start;
extern uint8_t *__data_end;
extern uint8_t *__heap_start;
extern uint8_t *__heap_end;
extern uint8_t *__bss_start;
extern uint8_t *__bss_end;

// Structure to represent RAM
struct MemProbe {
  int mTotal;      
  int mStaticDataStart; 
  int mStaticData;      // Global initialised
  int mStaticBSSStart;
  int mStaticBSS;       // Global un-initialised
  int mDynamic;   
  int mHeapStart;
  int mHeap;       // Dynamic (pointers and objects)
  int mHeapEnd;
  int mFree;       // Non-static - stack - heap
  int mSP;
  int mStack;      // Local variables
  int mRAMEND;
};

// Sample the RAM and update MemProbe
void memProbe();

// Global variable to refer to
extern MemProbe __mem_probe__;

#endif