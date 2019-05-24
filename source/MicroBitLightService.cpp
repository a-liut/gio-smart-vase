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
  * Class definition for the custom MicroBit Light Service.
  * Provides a BLE service to remotely read the silicon light of the nRF51822.
  */
#include "MicroBitConfig.h"
#include "ble/UUID.h"

#include "MicroBitLightService.h"

/**
  * Constructor.
  * Create a representation of the LightService
  * @param _ble The instance of a BLE device that we're running on.
  * @param _display An instance of MicroBitDisplay to use as our display source.
  */
MicroBitLightService::MicroBitLightService(BLEDevice &_ble, MicroBitDisplay &_display) :
        ble(_ble), display(_display)
{
    // Create the data structures that represent each of our characteristics in Soft Device.
    GattCharacteristic  lightDataCharacteristic(MicroBitLightServiceDataUUID, (uint8_t *)&lightDataCharacteristicBuffer, 0,
    sizeof(lightDataCharacteristicBuffer), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

    // Initialise our characteristic values.
    lightDataCharacteristicBuffer = display.readLightLevel();

    // Set default security requirements
    lightDataCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

    GattCharacteristic *characteristics[] = {&lightDataCharacteristic};
    GattService         service(MicroBitLightServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));

    ble.addService(service);

    lightDataCharacteristicHandle = lightDataCharacteristic.getValueHandle();

    ble.gattServer().write(lightDataCharacteristicHandle,(uint8_t *)&lightDataCharacteristicBuffer, sizeof(lightDataCharacteristicBuffer));

    ble.onDataWritten(this, &MicroBitLightService::onDataWritten);
    if (EventModel::defaultEventBus)
        EventModel::defaultEventBus->listen(MICROBIT_ID_DISPLAY, MICROBIT_DISPLAY_EVT_LIGHT_SENSE, this, &MicroBitLightService::lightUpdate, MESSAGE_BUS_LISTENER_IMMEDIATE);
}

/**
  * Light update callback
  */
void MicroBitLightService::lightUpdate(MicroBitEvent)
{
    if (ble.getGapState().connected)
    {
        lightDataCharacteristicBuffer = display.readLightLevel();
        ble.gattServer().notify(lightDataCharacteristicHandle,(uint8_t *)&lightDataCharacteristicBuffer, sizeof(lightDataCharacteristicBuffer));
    }
}

/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicroBitLightService::onDataWritten(const GattWriteCallbackParams *params)
{

}


const uint8_t  MicroBitLightServiceUUID[] = {
    0xe9,0x6d,0x61,0x00,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};

const uint8_t  MicroBitLightServiceDataUUID[] = {
    0xe9,0x6d,0x92,0x50,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};