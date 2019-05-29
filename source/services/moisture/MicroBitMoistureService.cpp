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
  * Class definition for the custom MicroBit Moisture Service.
  * Provides a BLE service to remotely read the moisture read by the micro:bit.
  */
#include "MicroBitConfig.h"
#include "ble/UUID.h"

#include "MicroBitMoistureService.h"
#include "../../sensors/moisture/MicroBitMoistureSensor.h"

/**
  * Constructor.
  * Create a representation of the MoistureService
  * @param _ble The instance of a BLE device that we're running on.
  * @param _sensor An instance of MicroBitMoistureSensor to use as our moisture source.
  */
MicroBitMoistureService::MicroBitMoistureService(BLEDevice &_ble, MicroBitMoistureSensor &_sensor) :
        ble(_ble), sensor(_sensor)
{
    // Create the data structures that represent each of our characteristics in Soft Device.
    GattCharacteristic  moistureDataCharacteristic(MicroBitMoistureServiceDataUUID, (uint8_t *)&moistureDataCharacteristicBuffer, 0,
    sizeof(moistureDataCharacteristicBuffer), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

    // Initialise our characteristic values.
    moistureDataCharacteristicBuffer = sensor.getMoistureLevel();

    // Set default security requirements
    moistureDataCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

    GattCharacteristic *characteristics[] = {&moistureDataCharacteristic};
    GattService         service(MicroBitMoistureServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));

    ble.addService(service);

    moistureDataCharacteristicHandle = moistureDataCharacteristic.getValueHandle();

    ble.gattServer().write(moistureDataCharacteristicHandle,(uint8_t *)&moistureDataCharacteristicBuffer, sizeof(moistureDataCharacteristicBuffer));

    if (EventModel::defaultEventBus)
        EventModel::defaultEventBus->listen(MICROBIT_ID_MOISTURE, MICROBIT_MOISTURE_EVT_UPDATE, this, &MicroBitMoistureService::moistureUpdate, MESSAGE_BUS_LISTENER_IMMEDIATE);
}

/**
  * Moisture update callback
  */
void MicroBitMoistureService::moistureUpdate(MicroBitEvent)
{
    if (ble.getGapState().connected)
    {
        moistureDataCharacteristicBuffer = sensor.getMoistureLevel();
        ble.gattServer().notify(moistureDataCharacteristicHandle,(uint8_t *)&moistureDataCharacteristicBuffer, sizeof(moistureDataCharacteristicBuffer));
    }
}

// 73cd5e04d32c4345a543487435c70c48
const uint8_t  MicroBitMoistureServiceUUID[] = {
    0x73,0xcd,0x5e,0x04,0xd3,0x2c,0x43,0x45,0xa5,0x43,0x48,0x74,0x35,0xc7,0x0c,0x48
};

// 73cd7350d32c4345a543487435c70c48
const uint8_t  MicroBitMoistureServiceDataUUID[] = {
    0x73,0xcd,0x73,0x50,0xd3,0x2c,0x43,0x45,0xa5,0x43,0x48,0x74,0x35,0xc7,0x0c,0x48
};