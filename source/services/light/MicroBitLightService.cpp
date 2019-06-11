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
  * Provides a BLE service to remotely read the light on the micro:bit display.
  */
#include "MicroBitConfig.h"
#include "ble/UUID.h"

#include "MicroBitLightService.h"
#include "MicroBitSystemTimer.h"

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
    lastUpdate = system_timer_current_time();

    // Set default security requirements
    lightDataCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

    GattCharacteristic *characteristics[] = {&lightDataCharacteristic};
    GattService         service(MicroBitLightServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));

    ble.addService(service);

    lightDataCharacteristicHandle = lightDataCharacteristic.getValueHandle();

    ble.gattServer().write(lightDataCharacteristicHandle,(uint8_t *)&lightDataCharacteristicBuffer, sizeof(lightDataCharacteristicBuffer));

    if (EventModel::defaultEventBus)
        EventModel::defaultEventBus->listen(MICROBIT_ID_DISPLAY, MICROBIT_DISPLAY_EVT_LIGHT_SENSE, this, &MicroBitLightService::lightUpdate, MESSAGE_BUS_LISTENER_IMMEDIATE);
}

/**
  * Light update callback
  */
void MicroBitLightService::lightUpdate(MicroBitEvent)
{
    if (ble.getGapState().connected && lastUpdate + MICROBIT_LIGHT_SERVICE_PERIOD <= system_timer_current_time())
    {
        lightDataCharacteristicBuffer = display.readLightLevel();
        ble.gattServer().notify(lightDataCharacteristicHandle,(uint8_t *)&lightDataCharacteristicBuffer, sizeof(lightDataCharacteristicBuffer));

        lastUpdate = system_timer_current_time();
    }
}

// 02751625523e493b8f941765effa1b20
const uint8_t  MicroBitLightServiceUUID[] = {
    0x02,0x75,0x16,0x25,0x52,0x3e,0x49,0x3b,0x8f,0x94,0x17,0x65,0xef,0xfa,0x1b,0x20
};

// 02759250523e493b8f941765effa1b20
const uint8_t  MicroBitLightServiceDataUUID[] = {
    0x02,0x75,0x92,0x50,0x52,0x3e,0x49,0x3b,0x8f,0x94,0x17,0x65,0xef,0xfa,0x1b,0x20
};