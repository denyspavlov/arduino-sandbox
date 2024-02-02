#include "Arduino.h"
#include "MemProbe.h"

/*
https://www.nongnu.org/avr-libc/user-manual/malloc.html
+---------------+------------------+---------------------------------------------+-----------------+
|               |                  |                                             |                 |
|               |                  |                                             |                 |
|    static     |                  |                                             |                 |
|     data      |       heap       |                   free ram                  |      stack      |
|               |                  |                                             |                 |
|               |                  |                                             |                 |
|               |                  |                                             |                 |
+---------------+------------------+---------------------------------------------+-----------------+
       _end or __heap_start     __brkval                                         SP             RAMEND
 */

MemProbe __mem_probe__ = { 
  (RAMEND - (int)&__data_start + 1),        // int mTotal;      
  ((int)&__data_start),                     // int mStaticDataStart; 
  ((int)&__data_end - (int)&__data_start),  // int mStaticData;      // Global initialised
  ((int)&__data_end),                       // int mStaticBSSStart;
  ((int)&__bss_end - (int)&__data_end),     // int mStaticBSS;       // Global un-initialised
  (((RAMEND - (int)&__data_start + 1)) - (((int)&__data_end - (int)&__data_start)) - (((int)&__bss_end - (int)&__data_end))), // int mDynamic;   
  ((int)&__bss_end), // int mHeapStart;
  0,                                        // int mHeap;       // Dynamic (pointers and objects)
  0,                                        // int mHeapEnd;
  0,                                        // int mFree;       // Non-static - stack - heap
  0,                                        // int mSP;
  0,                                        // int mStack;      // Local variables
  RAMEND                                    // int mRAMEND;
};

void memProbe() {
  __mem_probe__.mHeapEnd = (int) (__brkval == 0 ? (uint8_t *) &__heap_start : __brkval);
	//__mem_probe__.mHeapEnd = (int)SP - (int)&__malloc_margin;
  __mem_probe__.mHeap = __mem_probe__.mHeapEnd - __mem_probe__.mHeapStart;
  __mem_probe__.mSP = (int) SP;
  __mem_probe__.mStack = RAMEND - (int)SP + 1;
  __mem_probe__.mFree = __mem_probe__.mDynamic - __mem_probe__.mHeap - __mem_probe__.mStack;
}
