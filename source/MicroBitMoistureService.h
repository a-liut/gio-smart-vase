/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef MICROBIT_MOISTURE_SERVICE_H
#define MICROBIT_MOISTURE_SERVICE_H

#include "MicroBitConfig.h"
#include "ble/BLE.h"
#include "EventModel.h"
#include "MicroBitMoistureSensor.h"

// UUIDs for our service and characteristics
extern const uint8_t  MicroBitMoistureServiceUUID[];
extern const uint8_t  MicroBitMoistureServiceDataUUID[];

/**
  * Class definition for the custom MicroBit Temperature Service.
  * Provides a BLE service to remotely read the moisture level read by the micro:bit.
  */
class MicroBitMoistureService
{
    public:

    /**
      * Constructor.
      * Create a representation of the MicroBitMoistureService
      * @param _ble The instance of a BLE device that we're running on.
      * @param _sensor An instance of MicroBitMoistureSensor to use as our moisture source.
      */
    MicroBitMoistureService(BLEDevice &_ble, MicroBitMoistureSensor &_sensor);

    /**
     * Moisture update callback
     */
    void moistureUpdate(MicroBitEvent e);

    /**
     * Read the moisture level
     */
    int readMoisture();

    private:

    // Bluetooth stack we're running on.
    BLEDevice           	&ble;
    // Pins for reading moisture
    MicroBitMoistureSensor     &sensor;

    // memory for our 8 bit moisture characteristic.
    int8_t             moistureDataCharacteristicBuffer;

    // Handles to access each characteristic when they are held by Soft Device.
    GattAttribute::Handle_t moistureDataCharacteristicHandle;
};


#endif