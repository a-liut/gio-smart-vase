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

/**
  * Class definition for the custom MicroBit Watering Service.
  * Provides a BLE service to remotely read the watering read by the micro:bit.
  */
#include "MicroBitConfig.h"
#include "ble/UUID.h"
#include "MicroBitEvent.h"

#include "../../actuators/watering/MicroBitWateringActuator.h"
#include "MicroBitWateringService.h"

/**
  * Constructor.
  * Create a representation of the WateringService
  * @param _ble The instance of a BLE device that we're running on.
  * @param _actuator The instance of a MicroBitWateringActuator used to read watering status.
  */
MicroBitWateringService::MicroBitWateringService(BLEDevice &_ble, MicroBitWateringActuator &_actuator) :
        ble(_ble), actuator(_actuator)
{
    // Create the data structures that represent each of our characteristics in Soft Device.
    GattCharacteristic  wateringDataCharacteristic(MicroBitWateringServiceDataUUID, (uint8_t *)&wateringDataCharacteristicBuffer, 0,
    sizeof(wateringDataCharacteristicBuffer), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);

    // Initialise our characteristic values.
    wateringDataCharacteristicBuffer = actuator.isWatering();

    // Set default security requirements
    wateringDataCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

    GattCharacteristic *characteristics[] = {&wateringDataCharacteristic};
    GattService         service(MicroBitWateringServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));

    ble.addService(service);

    wateringDataCharacteristicHandle = wateringDataCharacteristic.getValueHandle();

    ble.gattServer().write(wateringDataCharacteristicHandle,(uint8_t *)&wateringDataCharacteristicBuffer, sizeof(wateringDataCharacteristicBuffer));

    ble.onDataWritten(this, &MicroBitWateringService::onDataWritten);
    if (EventModel::defaultEventBus)
        EventModel::defaultEventBus->listen(MICROBIT_ID_WATERING_ACTUATOR, MICROBIT_WATERING_ACTUATOR_EVT_UPDATE, this, &MicroBitWateringService::wateringUpdate, MESSAGE_BUS_LISTENER_IMMEDIATE);
}

/**
  * Watering update callback
  */
void MicroBitWateringService::wateringUpdate(MicroBitEvent)
{
    if (ble.getGapState().connected)
    {
        wateringDataCharacteristicBuffer = actuator.isWatering();
        ble.gattServer().notify(wateringDataCharacteristicHandle,(uint8_t *)&wateringDataCharacteristicBuffer, sizeof(wateringDataCharacteristicBuffer));
    }
}

/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicroBitWateringService::onDataWritten(const GattWriteCallbackParams *params)
{
    if (params->handle == wateringDataCharacteristicHandle && params->len >= sizeof(wateringDataCharacteristicBuffer))
    {
        wateringDataCharacteristicBuffer = *((uint16_t *)params->data);
        if(wateringDataCharacteristicBuffer)
        {
            // trigger watering
            MicroBitEvent evt = MicroBitEvent(MICROBIT_ID_WATERING_SERVICE, WATERING_EVT_REQUESTED);
            evt.fire();
        }

        wateringDataCharacteristicBuffer = actuator.isWatering();
        ble.gattServer().write(wateringDataCharacteristicHandle,(uint8_t *)&wateringDataCharacteristicBuffer, sizeof(wateringDataCharacteristicBuffer));
    }
}


// ce9eafe4c44341db9cb581e567f3ba93
const uint8_t  MicroBitWateringServiceUUID[] = {
    0xce,0x9e,0xaf,0xe4,0xc4,0x43,0x41,0xdb,0x9c,0xb5,0x81,0xe5,0x67,0xf3,0xba,0x93
};

// ce9e7625c44341db9cb581e567f3ba93
const uint8_t  MicroBitWateringServiceDataUUID[] = {
    0xce,0x9e,0x76,0x25,0xc4,0x43,0x41,0xdb,0x9c,0xb5,0x81,0xe5,0x67,0xf3,0xba,0x93
};