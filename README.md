# SOS-call-in-remote-areas

Imagine you going to hiking in a very wonderfull mountain, where the air is so clean thanks to the distance between you and the society. This is a very nice place to do a lot of things, such as a picnic, a painting... but no to fall and get your leg broken. If you see your phone in this situation, you will get a probably "no signal" message, that will make you know that this wasn't a very good idea to go so deep in the nature. 

At this moment you will probably think in a very useful device which can send SOS messages to the society and get the proper authorities attention. And this is what we do in this code (Or at leat in a very preliminar way).

We accomplished that using two boards:
- ESP32 with LoRa and BLE.
- ESP8266 Node MCU v3.

Both boards were designed to work as a stationary devices, the first one is whos manage the communication between your phone and send the long distance message (It will be positioned in concurred hiking points, with a distance between them and the other board less than 10km). And, in the other hand the ESP8266 receive this SOS message and report them to a web server, so its neccesary internet connection in this place.

I understand that it could be helpfull a mobile device to send these messages, and the way to replicate messages in this ESP32 devices to add a lot of coverage. But this repo was intended as a weekend project to apply the knowledge of a [IoT online course](https://platzi.com/cursos/iot-protocolos/), so if you want to create a commercial device this could be a good start point, or if you are interested in learning communication protocols (LoRa, BLE and MQTT) this project is for you.


### Needed material
In this proyect we have to boards which supply different misions:
- **ESP8266**, this ESP receive the info sended by another board in a LoRa protocol (Long ranges, up to 30km).
- **An ESP32 board with OLED, LoRa module, and BLE incorporated**, I used the [Heltec WiFi LoRa 32 v2](https://heltec.org/project/wifi-lora-32/ "Heltec WiFi LoRa 32 v2"). This is the board whos manage the communication between your phone and send the long distances messages.
- **A LoRa module**, I used the SX1278 at 433MHz with the Ra-01 Ai-thinker Chip.
- **WiFi router where the ESP8266 will be connected.**
- **Smarthphone**, I used an android with the [nRF Connect app](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=es_CO&gl=US).
- **A free account in [Cayenne my devices](https://accounts.mydevices.com/)**. Add your device and create a dashboard with a digital widget, [docs here](https://developers.mydevices.com/cayenne/docs/getting-started/). I used the virtual chanel number 1 to the communication.

### Deployment
- Connect your LoRa Module to the ESP8266 (Look your SPI GPIO pins), in my case:
| LoRa Module | ESP8266 GPIO pins |
|:-------------:|:-------------:|
| MISO | GPIO12 = D6 = HMISO |
| MOSI | GPIO13 = D7 = HMOSI |
| SCK | GPIO14 = D5 = HSCLK |
| NSS | GPIO15 = D8 = HCS |
| RST | GPIO16 = D0 |
| DIO0 | GPIO4 = D2 |
| GND | GROUND |
| 3V3 | 3V |

ss = GPIO15, rst = GPIO16, dio0=GPIO4.
- In the src/ folder you will find the ESP32 and the ESP8266 codes, add your WiFi ssid and password to get the ESP8266 connected. Add your cayenne my devices credentials. Upload the code to the corresponding boards. I used the arduino IDE for that process. Remember, if you use another board check the GPIO pins to SPI and the I2C.
- Download the nRF connect app to your phone (Every BLE generic app should work, but i didn't test it).
- Plug your ESP8266 to the power.
- Plug your ESP32 to the power and before that, connect your phone using the app.
- Write any value in the Unknown characteristic (UUID: f7c5104b-c698-42c9-8e76-aa421108805b), you can change this UUID using a generator as [https://www.uuidgenerator.net/](https://www.uuidgenerator.net/).
- If everything is ok, you will see in your online dashboard the emergency solicitation.
