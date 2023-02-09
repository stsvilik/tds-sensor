#include <HardwareSerial.h>
#include "src/TdsSensor.h"

#define TX0 17
#define RX0 16

HardwareSerial SerialPort(2);
TdsSensor tdsSensor(SerialPort);

void setup()
{
    SerialPort.begin(9600, SERIAL_8N1, RX0, TX0);
    Serial.begin(115200);

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