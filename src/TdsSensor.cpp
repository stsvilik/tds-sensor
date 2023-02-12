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

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#endif

#include "TdsSensor.h"

const byte MESSAGE_START = 0xAA;
const byte MESSAGE_END = 0x55;

void TdsSensor::setup()
{
    this->setup(ACTIVE);
}

void TdsSensor::setup(TDS_MODE mode)
{
    _mode = mode;
    byte *setupMessage;

    setupMessage = mode == ACTIVE ? ENABLE_ACTIVE_REPORTING : DISABLE_ACTIVE_REPORTING;

    this->sendCommand(setupMessage);
}

byte *TdsSensor::readSensorData()
{
    _tds1 = NULL;
    _tds2 = NULL;
    _temperature = NULL;

    if (_mode == PASSIVE)
    {
        /**
         * If active reporting is off, send manual report request and wait for response
         */
        this->sendCommand(REQUEST_REPORT);
    }

    this->readStartToTheEnd();

    if (newData == true)
    {
        newData = false;
    }

    return receivedData;
}

ushort TdsSensor::getTds()
{
    if (receivedData == nullptr)
    {
        return NULL;
    }

    _tds1 = ((uint16_t)receivedData[4] << 8) | receivedData[5];

    return _tds1;
}

ushort TdsSensor::getTds(TDS_PROBE probe)
{
    if (receivedData == nullptr)
    {
        return NULL;
    }

    _tds1 = ((uint16_t)receivedData[4] << 8) | receivedData[5];
    _tds2 = ((uint16_t)receivedData[6] << 8) | receivedData[7];

    return probe == TDS_2 ? _tds2 : _tds1;
}

ushort TdsSensor::getTemperature()
{
    if (receivedData == nullptr)
    {
        return NULL;
    }

    _temperature = receivedData[8];

    return _temperature;
}

void TdsSensor::sendCommand(byte *data)
{
    stream->write(data, COMMAND_LENGTH);
    stream->flush();
}

void TdsSensor::readStartToTheEnd()
{
    static boolean recvInProgress = false;
    static byte ndx = 0;
    boolean foundStart = false;
    byte rc;

    while (stream->available() > 0 && newData == false)
    {
        rc = stream->read();

        if (recvInProgress == true)
        {
            if (rc != MESSAGE_END)
            {
                receivedData[ndx++] = rc;
                if (ndx >= MESSAGE_LENGTH)
                {
                    ndx = MESSAGE_LENGTH - 1;
                }
            }
            else
            {
                receivedData[ndx] = rc;
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == MESSAGE_START)
        {
            receivedData[ndx++] = rc;
            recvInProgress = true;
        }
    }
}
