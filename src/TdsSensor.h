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

#ifndef TdsSensor_h
#define TdsSensor_h

#define MESSAGE_LENGTH 11

enum TDS_MODE
{
    ACTIVE,
    PASSIVE
};

enum TDS_PROBE
{
    TDS_1,
    TDS_2
};

class TdsSensor
{
private:
    /* data */
    byte DISABLE_ACTIVE_REPORTING[5] = {0xAA, 0x05, 0x00, 0x51, 0x55};
    byte ENABLE_ACTIVE_REPORTING[5] = {0xAA, 0x05, 0x01, 0x50, 0x55};
    byte REQUEST_REPORT[5] = {0xAA, 0x05, 0x02, 0x4F, 0x55};

    byte incomingMessage[MESSAGE_LENGTH];
    Stream *stream;
    ushort _tds1 = NULL;
    ushort _tds2 = NULL;
    ushort _temperature = NULL;
    TDS_MODE _mode = ACTIVE;

    void sendCommand(byte *data);
    void readStartToTheEnd();

public:
    TdsSensor(HardwareSerial &device) { stream = &device; };

    void setup();
    void setup(TDS_MODE mode);
    byte *readSensorData();
    ushort getTds();
    ushort getTds(TDS_PROBE probe);
    ushort getTemperature();
};

#endif