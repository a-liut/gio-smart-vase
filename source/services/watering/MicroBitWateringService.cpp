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
    sizeof(wateringDataCharacteristicBuffer), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

    // Initialise our characteristic values.
    wateringDataCharacteristicBuffer = actuator.isWatering();

    // Set default security requirements
    wateringDataCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

    GattCharacteristic *characteristics[] = {&wateringDataCharacteristic};
    GattService         service(MicroBitWateringServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));

    ble.addService(service);

    wateringDataCharacteristicHandle = wateringDataCharacteristic.getValueHandle();

    ble.gattServer().write(wateringDataCharacteristicHandle,(uint8_t *)&wateringDataCharacteristicBuffer, sizeof(wateringDataCharacteristicBuffer));

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


const uint8_t  MicroBitWateringServiceUUID[] = {
    0xe9,0x6d,0x71,0x00,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};

const uint8_t  MicroBitWateringServiceDataUUID[] = {
    0xe9,0x6d,0x72,0x50,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};