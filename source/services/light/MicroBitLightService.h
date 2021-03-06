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

#ifndef MICROBIT_LIGHT_SERVICE_H
#define MICROBIT_LIGHT_SERVICE_H

#define MICROBIT_LIGHT_SERVICE_PERIOD 5000

#include "MicroBitConfig.h"
#include "ble/BLE.h"
#include "MicroBitDisplay.h"
#include "EventModel.h"

// UUIDs for our service and characteristics
extern const uint8_t  MicroBitLightServiceUUID[];
extern const uint8_t  MicroBitLightServiceDataUUID[];

/**
  * Class definition for the custom MicroBit Temperature Service.
  * Provides a BLE service to remotely read the silicon temperature of the nRF51822.
  */
class MicroBitLightService
{
    public:

    /**
      * Constructor.
      * Create a representation of the TemperatureService
      * @param _ble The instance of a BLE device that we're running on.
      * @param _thermometer An instance of MicroBitThermometer to use as our temperature source.
      */
    MicroBitLightService(BLEDevice &_ble, MicroBitDisplay &_display);

    /**
     * Light update callback
     */
    void lightUpdate(MicroBitEvent e);

    private:

    // Bluetooth stack we're running on.
    BLEDevice           	&ble;
    MicroBitDisplay     &display;

    // Memory for our 8 bit temperature characteristic.
    int8_t             lightDataCharacteristicBuffer;

    // Last light update
    uint64_t lastUpdate;

    // Handles to access each characteristic when they are held by Soft Device.
    GattAttribute::Handle_t lightDataCharacteristicHandle;
};


#endif