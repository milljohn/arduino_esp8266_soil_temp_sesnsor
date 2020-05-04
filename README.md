# Arduino Soil Temperature Sensor

This began as a project for EE300 at New Mexico State University for the Fall 2018 semester. The scope of the project was to customize an existing Arduino based project using SMT components.

V0.1: Original design presented. Made use of an analog temperature sensor, LMT88. This worked correctly, however it required a voltage divider because the analog input on the ESP8266 is only 1V tolerant. This meant that there was a constant drain on the battery, even when the MCU was in deep sleep.

V0.2 Updated design

## Overview

- MCU: [ESP8266](https://www.espressif.com/sites/default/files/documentation/0a-esp8266ex_datasheet_en.pdf)
  - Minimal ciruitry to program and run
  - No on board USB to UART
- Sensor(s):
  - V0.1: [LMT88](https://www.ti.com/lit/ds/symlink/lmt88.pdf), other 3 pin analog sensor should work
  - V0.2: [DS18b20](https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf)

## Schematics

The Scematics are in the pcb directory. Current version is Soil Temp V0.02. The main difference between V0.01 and v0.02 is the temperature sensor.

## Programming

The program PostDataToGooleSheets takes a temp reading every hour, posts to Google Sheets, and then goes into deep sleep.

NOTE: Google sent me warnings about the sheet because it was public and eventually disabled public access.
