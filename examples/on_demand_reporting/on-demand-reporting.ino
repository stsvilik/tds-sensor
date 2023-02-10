/**
 * Copyright (c) 2023 Simon Tsvilik. All rights reserved.
 *
 * This file is part of Tds-Sensor.
 *
 * Tds-Sensor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Tds-Sensor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Tds-Sensor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <HardwareSerial.h>
#include <TdsSensor.h>

/** Using UART 2 default pins (ESP32) */
HardwareSerial SerialPort(2);
TdsSensor tdsSensor(SerialPort);

void setup()
{
    SerialPort.begin(9600, SERIAL_8N1);
    Serial.begin(115200);

    while (!SerialPort)
    {
        ; // wait for serial port to connect. Needed for native USB
    }
    /**
     * Passing PASSIVE mode argument will disable automatic reporting and instead will request a report every time readSensorData() is called.
     * NOTE: To return to automatic reporting, set mode to ACTIVE.
     */
    tdsSensor.setup(PASSIVE);
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