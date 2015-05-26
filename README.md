## ESP8266 based MQTT adapter for Max7219 based LED display.

Subscribes to ```/leddisp1/show``` and displays messages on my [Max7219 based display](https://github.com/aniline/arduino-sketchbook/tree/master/mqtt_display).

ESP communicates to an arduino through SPI. String messages of the form "N,M,<String>" are parsed where
* N is the timeout in seconds after which the arduino clears the display. 
* M is the mode, currently supports
  * 0 for stationary display
  * 1 for scrolling display



The same string is expected as the payload for the topic over MQTT. ESP sends only 32 bytes of the payload
to the display.

## Wiring

Arduino should be in 'slave' SPI mode. Appropriate level conversion should be used for 5V arduino. Almost
all of data flows from ESP to the arduino.

    ESP             |  Arduino
    ----------------+---------
    GPIO4           |  10 (SS)
    GPIO13 (SPID)   |  11 (MOSI)
    GPIO14 (SPICLK) |  13 (CLK)

The GPIO4 and GPIO5 might be inverted on some ESP modules (it was for my ESP-12).
My configuration works with arduino running the sketch in [aniline/arduino-sketchbook/mqtt_display](https://github.com/aniline/arduino-sketchbook/tree/master/mqtt_display).

## Building

Setup the variables as required by https://github.com/esp8266/source-code-examples . 
This includes stuff like:

    XTENSA_TOOLS_ROOT=/opt/Espressif/crosstool-NG/builds/xtensa-lx106-elf/bin
    SDK_BASE=/opt/Espressif/ESP8266_SDK
    FW_TOOL=${XTENSA_TOOLS_ROOT}/esptool
    ESPTOOL=${XTENSA_TOOLS_ROOT}/esptool.py
    ESPPORT=/dev/ttyUSB0

Build checkdirs because the Makefile symlinks some .c files from SDK in the first step.

    $ make checkdirs
    $ make

Flash using. Pass ```ESPPORT=<ESP serial port>``` if it changed from the time you set the environment.

    $ make flash

## Configuring

It uses the [tuanpmt's](https://github.com/tuanpmt) library and configuration mechanism. It loads
the settings in ```user_config.h``` to flash if the magic number CFG_HOLDER is different from whats
in the flash. So when you update configuration change CFG_HOLDER to re-save the values to flash.

## Thanks

* [tuanpmt's MQTT Library: esp_mqtt](https://github.com/tuanpmt/esp_mqtt) 
