# OLED

Multi OLED example using Multiplexer.

Esentially this is built upon the simple OLED, however due to I2C address
conflict it is not possible to use 2x OLED devices. A multiplexer is needed.

What multiplexer doesis that it acts as it's own I2C device with APIs to switch
between sub channels, thus avoiding the address collision on the wires.

It must be said that this was not an easy upgrade as the Adafruit library for OLEDs
had to be shared on the Arduino. Must be only one instance for some reason 
(array of instances does not work).
  
One idea was that this was caused due to memory issue and tried the code on Mega2560
with plenty of spare space (1 instance uses 1K). So there was 6K spare on mega but
when second OLED starts up the transfer data is scrambled, which makes me think it is
a problem with Adafruit_SSD1306 library (potentially some global variables sharing).
 
However I got it to working with just one instance with a few slightly odd behaviour
It seems like if the displays are not fully updated data from one device prints on
the second one. This is why the "swap" function was introduced.

In conclusion you can have multi-display but you need to refresh full screen after
swapping the channel, which means that full display state has to be stored on 
Arduino.

Having some thoughts on how this ties with other devices, it seems to me that 
intergating wultiple OLEDs would work better through SPI connected to one Arduino
that would mediate all the screens and provide a simple interface to update them,
a sort of facade where simple instructions can be passed to it: 
e.g. $display,x,y,sometext. Or make purposeful modules with OLED setup that "know"
how the display needs to be (say 2nd OLED right bottom corner is timer), so that
master I2C device could send to slave I2C just the time: e.g. $time:xxxx and then
the slave would know exact which display and which section to update say via SPI.


*Wokwi*
No multiplexer

*Components*
Arduino Pro Mini - Compatible 5V ATmega328 16MHz Board
https://www.ebay.co.uk/itm/262945527227?var=561951098804
0.96" OLED 128X64 I2C White Display Arduino SSD1306
https://www.ebay.co.uk/itm/263574014445?var=562623326218
TCA9548A I2C Multiplexer
https://www.ebay.co.uk/itm/142502915782

*PC Connector*
ATmega328P-PU Microcontroller MCU AVR 32K 20MHz FTDI USB to TTL 6Pin FT232RL
https://www.ebay.co.uk/itm/155945752144?var=456375192734

 