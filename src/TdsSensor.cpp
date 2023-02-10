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

    if (stream->available())
    {
        if (stream->peek() != MESSAGE_START)
        {
            (void)stream->read();

            return nullptr;
        }

        stream->readBytes(incomingMessage, MESSAGE_LENGTH);

        if (incomingMessage[MESSAGE_LENGTH - 1] == MESSAGE_END)
        {
            this->_tds1 = ((uint16_t)incomingMessage[4] << 8) | incomingMessage[5];
            this->_tds2 = ((uint16_t)incomingMessage[6] << 8) | incomingMessage[7];

            this->_temperature = incomingMessage[8];

            return incomingMessage;
        }
    }

    return nullptr;
}

ushort TdsSensor::getTds()
{
    return this->_tds1;
}

ushort TdsSensor::getTds(TDS_PROBE probe)
{
    return probe == TDS_2 ? this->_tds2 : this->_tds1;
}

ushort TdsSensor::getTemperature()
{
    return this->_temperature;
}

void TdsSensor::sendCommand(byte *data)
{
    stream->write(data, sizeof(data));
}