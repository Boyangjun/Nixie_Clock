First of all, what is a nixie clock?

They are essentially timekeeping clocks, but nixie clock uses **nixie tube** to show the time. Nixie tube is a kind of ion tube that uses the principle of gas nixie discharge to work, the color of nixie determines by the composition of gas filled in the tube, such as neon to red,  argon to light purple, mercury to light blue, helium to pink and so on.

Nixie tube has some characteristics, such as power supply of 170V (so you need to do special boost), it’s life span is about one or two years, and keeping the number to tubes for a long time may lead to the cathode of tube poisoning(harmless for humans), so if you want to display the time, you need to do some small actions to change the numbers.

Light nixie is comfortable to look at, it will be a cool clock if together with RGB, Wi-Fi, temperature and humidity sensors, it should be a cool clock.


## Component Selection

* **Nixie Tube** I chose the IN-14 from the former Soviet Union, a new one costs 100 yuan, and a second-hand costs 50 yuan. In addition to the 0-9 numbers, there are left and right decimal points to show. It has a nice color and high quality.
* **Main control chip**I chose Arduino atmega 328p , because it can be developed directly on Arduino IDE, which is convenient for Mac computers, and there are extension libraries that can simplify programming(although it’s not a good thing for learning)
* **Time Receive** I chose esp8266 12F to obtain global standard time via ump connection of NTP Server
* **Boost**I just buy others boost module which can boost 5V to 170V, I recommend to design by yourself but I don’t have time.
* **RGB**I chose ws2812, which is a common one, it show fast and accurate and can be develop easily by Arduino IDE.
* PS: I didn’t use **Time Chip** like DS1302, Its really a failure, if the connection to NTP Server is not good, time shows in a bad way like jumping a few seconds, and if no Wi-Fi, it is unable to obtain time. To solve this problem, I added  DS3231 time count module, so it only need to get time when switching on, even if the power cut off, the time can be continued by button battery power.

## Circuit Design

* Nixie Clock Driver: 74HC595 + A42triode, easy to design and saving IO ports.

## Program Design

Program consist of two parts: setup() and loop(), the former only run once after power on, while loop() runs once and once until power off. So setup() contains some initialize likeWi-Fi connection, NTP Server connection, RGB, etc. While Loop() keeps receiving time  every second (or faster) and some other functions.

Some great libraries here, Thanks to their author!

Wifi and NTP Server：https://github.com/bportaluri/WiFiEsp

75HC595 Driver：[GitHub - Simsso/ShiftRegister74HC595: Arduino library that simplifies the usage of shift registers](https://github.com/Simsso/ShiftRegister74HC595)

RGB Driver：https://github.com/adafruit/Adafruit_NeoPixel

## Finally
I really hope this will help you for designing nixie clock. Any questions or suggestions, e-mail me! (boyangjun@bupt.edu.cn)



[image:BA12CFBF-97E2-4782-B7CB-3B10783BA695-6988-0000322A6EA5FA8B/IMG_20190530_185942.jpg]
