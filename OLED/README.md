# OLED

Simple OLED example.

*Wokwi*
I2C version only
https://wokwi.com/projects/387919206799139841

Dual version is controlled through I2C constant, if commented out will use SPI display

Connections:
| Arduino Pro Mini | OLED I2C (4 pin) | OLED SPI (7 pin) |
| ---------------- |:----------------:| ----------------:|
| ACC              | VCC              | VCC              |
| GND              | GND              | GND              |
| A5 (I2C SCL)     | SCL              |                  |
| A4 (I2C SDA)     | SDA              |                  |
| 13 (SPI SCK)     |                  | D0               |
| 11 (SPI MOSI)    |                  | D1               |
| 8                |                  | RES              |
| 7                |                  | CS               |
| 6                |                  | DC               |

NOTE: Use Adafruit Hardware SPI version (defaulkt sketch uses software SPI for UNO)

*Components*
Arduino Pro Mini - Compatible 5V ATmega328 16MHz Board
https://www.ebay.co.uk/itm/262945527227?var=561951098804
0.96" OLED 128X64 I2C White Display Arduino SSD1306
https://www.ebay.co.uk/itm/263574014445?var=562623326218

*PC Connector*
ATmega328P-PU Microcontroller MCU AVR 32K 20MHz FTDI USB to TTL 6Pin FT232RL
https://www.ebay.co.uk/itm/155945752144?var=456375192734

 