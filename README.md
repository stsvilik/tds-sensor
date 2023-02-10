# Tds-Sensor

Arduino library for communicating with a TDS Sensors which feature a built-in processing microcontroller. This library is NOT compatible with analog TDS sensors that require ADC conversion.

## What this library is NOT

There are plenty of analog TDS Sensors and their clones circulating the internet such as DFRobot / Gravity, Grove, CQRobot, etc. which supposedly work with Arduino. I found those sensors extremely unreliable and uncalibrated, also heavily reliant on flaky ADC controllers which are built into Arduino devices. Even with external/dedicated ADC the readings are not relable.

If you have one of those sensors, this library would not work for you :)

## About this library

This library is designed to work with few sensors circulating Alibaba which feature a built-in microcontroller that does all the work of calculating the TDS value and also features a built-in thermometer to adjust TDS measurements accordingly. Most of these devices are pre-calibrated at the factory. Some of these sensors even feature a multi-TDS meter interface that would allow you to take measurement from one or more sensors. These microcontrollers communicate over serial interface (UART). Most Arduino devices feature several programmable serial ports which you could use to capturethe data.

## Specifications

**NOTE:** Not all the sensors come equipped with built-in temperature sensor, so use applicable specs below.

|        |        |
| ------ | ------ |
| Power Requirements | 3~5 VDC |
| Operating Current  | Normal operating current< 5mA, maximum < 10mA |
| Detection Range | 0~2000 ppm |
| Resolution | 1 ppm |
| Response time | \< 1s |
| Communication | UART, baud rate 9600, data bits 8 bits, 1 stop bit, no validation |
| Temperature sensing range | 0~99℃ |
| Temperature resolution | 1℃ |
| Temperature accuracy | ± 1℃ |

## Modes of operation

On-board TDS microcontrollers support two modes; `ACTIVE` (default) and `PASSIVE`. In `ACTIVE` mode controller will report TDS/Temperature data once every 5 minutes. In `PASSIVE` mode, controller will not report automatically and instead will only report TDS/Temperature data on-demand, by calling `readSensorData()` method.

**NOTE**: When sensor is just powered on (and mode is left at default), it will report once every second for about a minute and afterwards will switch to automatic reporting (once/5 min).

## Example

```c++
#include <HardwareSerial.h>
#include <TdsSensor.h>

HardwareSerial SerialPort(2);
TdsSensor tdsSensor(SerialPort);

void setup()
{
    SerialPort.begin(9600, SERIAL_8N1, RX0, TX0);
    Serial.begin(115200);

    while (!SerialPort)
    {
        ; // wait for serial port to connect. Needed for native USB
    }

    tdsSensor.setup();
}

void loop()
{
    const byte *data = tdsSensor.readSensorData();

    ushort tds = tdsSensor.getTds();
    ushort temp = tdsSensor.getTemperature();

    if (data != nullptr)
    {
        Serial.printf("TDS: %d @ %d celcius\n", tds, temp);
    }

    delay(1000);
}

```


