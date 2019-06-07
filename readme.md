# 辉光钟
#个人 #辉光钟
首先，什么叫辉光钟？

本质上都是授时的钟表，不过辉光钟是用**辉光管**来显示时间的。辉光管是一种利用气体辉光放电原理而工作的的离子管，辉光的颜色决定于管内所充气体的成分，如氖显红色，氩显浅紫色，汞显淡蓝色，氦显粉红色等。

辉光管有一些特性，比如需要170V才能驱动，所以需要专门做升压；它的寿命大概是一两年，而且管子的数字长时间不变的话容易导致管子的阴极中毒（不是人中毒，应该就是管子显示不了之类的），所以显示时间的话，要做一些小动作，比如显示小时的两个管子时不时就要让它动一下。

![](readme/59c5245d60f5b9804.jpg_e680.jpg)

淡淡的辉光能让人看着很舒服，加上RGB、Wi-Fi、温湿度传感器等应该会成为一个很酷的钟，在宿舍就是自己独享的moment（大雾）

## 选型
* **辉光管**我选了前苏联的IN-14，新的100块一根，旧的50块一个，除了0-9还有左右小数点可以显示，它的颜色很好看，质量比较高。
* **主控**我选了Arduino atmega328p，因为它可以直接在Arduino IDE 上进行开发，这对于MAC来说很方便，有拓展库可以简化编程（虽然这对学习不是一件好事）
* **授时**我选了esp8266的12-F模块，通过udp连接上ntp服务器获取全球标准时间（比北京时间晚了8小时，获取后小时+8即可）
* **升压**直接买的模块，效率和模块大小我都比较满意，但是终究不是自己做的心里不舒服，有时间还是建议自己设计
* **RGB**选的ws2812，这个就是很常用的RGB，显色快速准确，而且arduino IDE开发方便。
* PS：我第一版没有用*时钟芯片*，这实在是一个败笔，如果连接的ntp服务器不行，获取时间有时候就会出现卡顿、延迟等现象，效果不好，而且没有Wi-Fi的时候就显示不了时间了，之后我就加上了DS3231来做时钟模块，加上后只需要开机获取一次时间就可以了，即使断电了也可以通过纽扣电池供电继续计时。

## 电路设计
* 辉光管驱动：使用74HC595（串并转换芯片）+A42三极管（耐高压）来驱动。用他们的原因是74HC595用3个IO口来操作很多很多辉光管，三极管起一个开关作用。

![](https://graph.baidu.com/resource/101e0a26cbf5037749ff601559876775.jpg)

![](readme/C2F3AB36-9C6C-4D71-A8EB-A35E30C54E8C.png)
* 最小系统看使用的主控而定；用了一个转串口芯片CH340使得可以用usb烧写程序，usb口处放一个自恢复保险丝来做一些电路保护。

## 程序设计
程序分两段，setup和loop，前者在电路上电后只会执行一次，后者在setup后会一直循环执行，所以setup主要做的事情是连接Wi-Fi，连接ntp服务器，开机RGB显示等操作，loop主要做的是每隔1s（或者更短）获取一次时间，以及防止阴极中毒的措施，以及运行时RGB显示等操作。

这里提供一些好用的库，也想他们的作者表示感谢！

Wifi联网+连接ntp服务器：https://github.com/bportaluri/WiFiEsp

75HC595驱动：[GitHub - Simsso/ShiftRegister74HC595: Arduino library that simplifies the usage of shift registers](https://github.com/Simsso/ShiftRegister74HC595)

RGB驱动：https://github.com/adafruit/Adafruit_NeoPixel

## 写在最后
希望能给想做辉光钟的同学提供一些帮助，如果有建议或者疑问，可以给我发邮件(boyangjun@bupt.edu.cn)



![](readme/IMG_20190530_185942.jpg)
