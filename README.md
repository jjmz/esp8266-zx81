# esp8266-zx81

Using a sledgehammer to crack a nut : 

        Blinky on esp8266 via Z80 emulator running ZX81 Basic (ROM)

The esp8266 is running a Z80 emulator (code : anotherlin/z80emu)
and the ZX81 ROM (open81).

Keyboard/Display currently via UDP on port 1025, with a Python Client on a PC.

Poke outside of the ZX81 RAM (@33333, for example) is used by GPIO2 (LSB).

Built using esp-open-sdk, and esp_iot_sdk_v1.3.0.

JJM

Just add a file "wifi-params.h" in folder user, with your wireless credentials

    #define WIFI_SSID "YourSSID"
    #define WIFI_PASSWD "YourKeyPwd"

