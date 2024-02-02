# Sonar

This is a simple sonar station project that combines a few components to have a bit of fun.
Components used are:
* OLED I2C version
* Stock starter kit Servo SG90
* Stock starter kit untra sonic sensor HC-SR04 
* Toggle button with pull up resistor

The objective is to have a sonar station that will rotate 0-180 degrees (using servo) and
record distance readings (using ultra sonic sensor). The sweep is initiated by pressing a
push button that acts as a toggle switch. Once the sweeping starts OLED display will plot
the dots of the potential objects detected and create a visual map of the "wall".

Specification:
* Servo will sweep at full degree range with 1 degree step
* At each step utlra sonic sensor will take a reading. The reading should take at minimum 20mS 
  but can be as long as 50 millis (timeout). 
* The distance is calculated using formula pulse time * 340m/s (speed of sound) / 2 (since it is round trip) 
  This is using standard sound speed at 20C in a dry weather
* Each distance measurement is stored in an array[180], one element for each degree of reading
* Distance data is re-mapped to x,y coordinates for the 124x68 OLED screen. SONAR_RANGE constant
  is set to 100 and defines visible range on OLED (i.e. max distance plotted is 100cm). 
  The mapping uses formula: 
      
	  RAD = distance * PI / 180, 
	  x = distance * cos(RAD), 
	  y = distance * sin(RAD).
	  
  This gives the x,y coordinate of a map where each unit is cm. These coordinates are then re-mapped
  to OLED coordinates giving 400/100=4cm per pixel precision:
   
  	  // Origin on OLED is 62,64 (top left corner is 0,0)
      x = map(x, -SONAR_RANGE, SONAR_RANGE, 2, SCREEN_WIDTH - 2), 
	  // Top is 0, bottom is 62 
	  y = map(y, 0, SONAR_RANGE, SCREEN_HEIGHT - 2, 2); 

*Wokwi*
https://wokwi.com/projects/388634972887675905

*Components*
Arduino Mega

0.96" OLED 128X64 I2C White Display Arduino SSD1306
https://www.ebay.co.uk/itm/263574014445?var=562623326218

Ultra sonic HC-SR04

Servo SG90

Push button + 10K pull up resistor

LED + 220 resistor

Some LEGO structure elements
 