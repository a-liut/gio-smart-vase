#include "MicroBit.h"
#include "MicroBitUARTService.h"
#include "MicroBitLightService.h"
#include "MicroBitTemperatureService.h"
#include "MicroBitMoistureSensor.h"
#include "MicroBitMoistureService.h"

#define UPDATE_TIMEOUT 2000

#define WATERING_EVENT_ID 55536 // Reserved events: [1-65535]
#define WATERING_EVENT_REQUESTED 1
#define WATERING_EVENT_STARTED 2
#define WATERING_EVENT_ENDED 3
#define WATERING_TIMEOUT 2000

#define VALUE_LABEL_TEMPERATURE ManagedString("temperature")
#define VALUE_LABEL_MOISTURE ManagedString("moisture")
#define VALUE_LABEL_LIGHT ManagedString("light")

MicroBit uBit;
MicroBitLightService *lightService;
MicroBitTemperatureService *temperatureService;
MicroBitMoistureService *moistureService;

MicroBitMoistureSensor moistureSensor(uBit.io.P0, uBit.io.P1);

int connected = 0;
int watering = 0;

// Device data
int temperature = 0;
int moisture = 0;
int light = 0;

void updateMeasurements()
{
    // Temperature
    temperature = uBit.thermometer.getTemperature();

    // Light
    light = uBit.display.readLightLevel();

    // Moisture
    uBit.io.P1.setAnalogValue(1023);
    moisture = uBit.io.P0.getAnalogValue();
    uBit.io.P1.setAnalogValue(0);
}

void onDisconnected(MicroBitEvent)
{
    uBit.display.scroll("D");
    connected = 0;
}

int canWater()
{
    return watering == 0;
}

void onWateringRequested(MicroBitEvent)
{
    if (canWater())
    {
        watering = 1;

        // Start the pump
        uBit.io.P2.setDigitalValue(1);

        // Wait
        uBit.sleep(WATERING_TIMEOUT);

        // Stop the pump
        uBit.io.P2.setDigitalValue(0);
    }
}

// void onWateringStarted(MicroBitEvent evt)
// {

// }

// void onWateringEnded(MicroBitEvent evt)
// {

// }

void init()
{
    // Initialise the micro:bit runtime.
    uBit.init();

    // Calibrate termometer
    uBit.thermometer.setCalibration(uBit.thermometer.getTemperature());

    // This will returns 0 on the first call
    uBit.display.readLightLevel();
}

int main()
{
    init();

    // BLE setup
    // uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    uBit.messageBus.listen(WATERING_EVENT_ID, WATERING_EVENT_REQUESTED, onWateringRequested);
    // uBit.messageBus.listen(WATERING_EVENT_ID, WATERING_EVENT_STARTED, onWateringStarted);
    // uBit.messageBus.listen(WATERING_EVENT_ID, WATERING_EVENT_ENDED, onWateringEnded);

    lightService = new MicroBitLightService(*uBit.ble, uBit.display);
    temperatureService = new MicroBitTemperatureService(*uBit.ble, uBit.thermometer);
    moistureService = new MicroBitMoistureService(*uBit.ble, moistureSensor);

    while (1)
    {
        if (connected == 0) {
            uBit.display.scroll("W");
            updateMeasurements();

            if (canWater())
            {
                MicroBitEvent evt = MicroBitEvent(WATERING_EVENT_ID, WATERING_EVENT_REQUESTED);
                evt.fire();
            }

            uBit.sleep(UPDATE_TIMEOUT);
        }
    }

    release_fiber();
}
