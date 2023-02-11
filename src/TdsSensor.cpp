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

byte MESSAGE_START = 0xAA;
byte MESSAGE_END = 0x55;
boolean newData = false;
byte receivedData[MESSAGE_LENGTH];

void TdsSensor::setup()
{
    this->setup(ACTIVE);
}

void TdsSensor::setup(TDS_MODE mode)
{
    this->_mode = mode;
    byte *setupMessage;

    setupMessage = mode == ACTIVE ? ENABLE_ACTIVE_REPORTING : DISABLE_ACTIVE_REPORTING;

    this->sendCommand(setupMessage);
}

byte *TdsSensor::readSensorData()
{
    this->_tds1 = NULL;
    this->_tds2 = NULL;
    this->_temperature = NULL;

    if (this->_mode == PASSIVE)
    {
        /**
         * If active reporting is off, send manual report request and wait for response
         */
        this->sendCommand(REQUEST_REPORT);

        while (!stream->available())
        {
        }
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
    this->_tds1 = ((uint16_t)receivedData[4] << 8) | receivedData[5];

    return this->_tds1;
}

ushort TdsSensor::getTds(TDS_PROBE probe)
{
    this->_tds1 = ((uint16_t)receivedData[4] << 8) | receivedData[5];
    this->_tds2 = ((uint16_t)receivedData[6] << 8) | receivedData[7];

    return probe == TDS_2 ? this->_tds2 : this->_tds1;
}

ushort TdsSensor::getTemperature()
{
    this->_temperature = receivedData[8];

    return this->_temperature;
}

void TdsSensor::sendCommand(byte *data)
{
    if(stream->availableForWrite() >= sizeof(data)) {
        stream->write(data, sizeof(data));
    } else {
        stream->flush();
        stream->write(data, sizeof(data));
    }
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
