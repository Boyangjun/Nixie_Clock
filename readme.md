# Nixie Clock
#个人 #辉光钟
首先，什么叫辉光钟？
First of all, what is a nixie clock?

本质上都是授时的钟表，不过辉光钟是用**辉光管**来显示时间的。辉光管是一种利用气体辉光放电原理而工作的的离子管，辉光的颜色决定于管内所充气体的成分，如氖显红色，氩显浅紫色，汞显淡蓝色，氦显粉红色等。
They are essentially timekeeping clocks, but nixie clock uses **nixie tube** to show the time. Nixie tube is a kind of ion tube that uses the principle of gas nixie discharge to work, the color of nixie determines by the composition of gas filled in the tube, such as neon to red,  argon to light purple, mercury to light blue, helium to pink and so on.

辉光管有一些特性，比如需要170V才能驱动，所以需要专门做升压；它的寿命大概是一两年，而且管子的数字长时间不变的话容易导致管子的阴极中毒（不是人中毒，应该就是管子显示不了之类的），所以显示时间的话，要做一些小动作，比如显示小时的两个管子时不时就要让它动一下。
Nixie tube has some characteristics, such as power supply of 170V (so you need to do special boost), it’s life span is about one or two years, and keeping the number to tubes for a long time may lead to the cathode of tube poisoning(harmless for humans), so if you want to display the time, you need to do some small actions to change the numbers.

淡淡的辉光能让人看着很舒服，加上RGB、Wi-Fi、温湿度传感器等应该会成为一个很酷的钟，在宿舍就是自己独享的moment（大雾）
Light nixie is comfortable to look at, it will be a cool clock if together with RGB, Wi-Fi, temperature and humidity sensors, it should be a cool clock.


## Component Selection
* **辉光管**我选了前苏联的IN-14，新的100块一根，旧的50块一个，除了0-9还有左右小数点可以显示，它的颜色很好看，质量比较高。
* **主控**我选了Arduino atmega328p，因为它可以直接在Arduino IDE 上进行开发，这对于MAC来说很方便，有拓展库可以简化编程（虽然这对学习不是一件好事）
* **授时**我选了esp8266的12-F模块，通过udp连接上ntp服务器获取全球标准时间（比北京时间晚了8小时，获取后小时+8即可）
* **升压**直接买的模块，效率和模块大小我都比较满意，但是终究不是自己做的心里不舒服，有时间还是建议自己设计
* **RGB**选的ws2812，这个就是很常用的RGB，显色快速准确，而且arduino IDE开发方便。
* PS：我第一版没有用**时钟芯片**，这实在是一个败笔，如果连接的ntp服务器不行，获取时间有时候就会出现卡顿、延迟等现象，效果不好，而且没有Wi-Fi的时候就显示不了时间了，之后我就加上了DS3231来做时钟模块，加上后只需要开机获取一次时间就可以了，即使断电了也可以通过纽扣电池供电继续计时。
* **Nixie Tube** I chose the IN-14 from the former Soviet Union, a new one costs 100 yuan, and a second-hand costs 50 yuan. In addition to the 0-9 numbers, there are left and right decimal points to show. It has a nice color and high quality.
* **Main control chip**I chose Arduino atmega 328p , because it can be developed directly on Arduino IDE, which is convenient for Mac computers, and there are extension libraries that can simplify programming(although it’s not a good thing for learning)
* **Time Receive** I chose esp8266 12F to obtain global standard time via ump connection of NTP Server
* **Boost**I just buy others boost module which can boost 5V to 170V, I recommend to design by yourself but I don’t have time.
* **RGB**I chose ws2812, which is a common one, it show fast and accurate and can be develop easily by Arduino IDE.
* PS: I didn’t use **Time Chip** like DS1302, Its really a failure, if the connection to NTP Server is not good, time shows in a bad way like jumping a few seconds, and if no Wi-Fi, it is unable to obtain time. To solve this problem, I added  DS3231 time count module, so it only need to get time when switching on, even if the power cut off, the time can be continued by button battery power.

## Circuit Design
* 辉光管驱动：使用74HC595（串并转换芯片）+A42三极管（耐高压）来驱动。用他们的原因是74HC595用3个IO口来操作很多很多辉光管，三极管起一个开关作用。

* 最小系统看使用的主控而定；用了一个转串口芯片CH340使得可以用usb烧写程序，usb口处放一个自恢复保险丝来做一些电路保护。
* Nixie Clock Driver: 74HC595 + A42triode, easy to design and saving IO ports.

## Program Design
程序分两段，setup和loop，前者在电路上电后只会执行一次，后者在setup后会一直循环执行，所以setup主要做的事情是连接Wi-Fi，连接ntp服务器，开机RGB显示等操作，loop主要做的是每隔1s（或者更短）获取一次时间，以及防止阴极中毒的措施，以及运行时RGB显示等操作。
Program consist of two parts: setup() and loop(), the former only run once after power on, while loop() runs once and once until power off. So setup() contains some initialize likeWi-Fi connection, NTP Server connection, RGB, etc. While Loop() keeps receiving time  every second (or faster) and some other functions.

这里提供一些好用的库，也想他们的作者表示感谢！
Some great libraries here, Thanks to their author!

Wifi and NTP Server：https://github.com/bportaluri/WiFiEsp

75HC595 Driver：[GitHub - Simsso/ShiftRegister74HC595: Arduino library that simplifies the usage of shift registers](https://github.com/Simsso/ShiftRegister74HC595)

RGB Driver：https://github.com/adafruit/Adafruit_NeoPixel

## 写在最后
希望能给想做辉光钟的同学提供一些帮助，如果有建议或者疑问，可以给我发邮件(boyangjun@bupt.edu.cn)
I really hope this will help you for designing nixie clock. Any questions or suggestions, e-mail me! (boyangjun@bupt.edu.cn)



[image:BA12CFBF-97E2-4782-B7CB-3B10783BA695-6988-0000322A6EA5FA8B/IMG_20190530_185942.jpg]
