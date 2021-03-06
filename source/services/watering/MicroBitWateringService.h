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

#ifndef MICROBIT_WATERING_SERVICE_H
#define MICROBIT_WATERING_SERVICE_H

#include "MicroBitConfig.h"
#include "ble/BLE.h"
#include "EventModel.h"

#include "../../actuators/watering/MicroBitWateringActuator.h"

#define MICROBIT_ID_WATERING_SERVICE          1334
#define WATERING_EVT_REQUESTED                42

// UUIDs for our service and characteristics
extern const uint8_t  MicroBitWateringServiceUUID[];
extern const uint8_t  MicroBitWateringServiceDataUUID[];

/**
  * Class definition for the custom MicroBit Watering Service.
  * Provides a BLE service to remotely read the watering activity performed by the micro:bit.
  */
class MicroBitWateringService
{
    public:

    /**
      * Constructor.
      * Create a representation of the WateringService
      * @param _ble The instance of a BLE device that we're running on.
      * @param _actuator The instance of a MicroBitWateringActuator used to read watering status.
      */
    MicroBitWateringService(BLEDevice &_ble, MicroBitWateringActuator &_actuator);

    /**
     * Watering update callback
     */
    void wateringUpdate(MicroBitEvent e);
    
    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    void onDataWritten(const GattWriteCallbackParams *params);

    private:

    // Bluetooth stack we're running on.
    BLEDevice           	&ble;

    // Actuator used to read the watering process status
    MicroBitWateringActuator      &actuator;

    // memory for our 8 bit watering characteristic.
    int8_t             wateringDataCharacteristicBuffer;

    // Handles to access each characteristic when they are held by Soft Device.
    GattAttribute::Handle_t wateringDataCharacteristicHandle;
};


#endif