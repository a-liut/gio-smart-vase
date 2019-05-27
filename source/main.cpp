#include "MicroBit.h"
#include "MicroBitUARTService.h"
#include "MicroBitTemperatureService.h"
#include "services/light/MicroBitLightService.h"

#include "sensors/moisture/MicroBitMoistureSensor.h"
#include "services/moisture/MicroBitMoistureService.h"

#include "actuators/watering/MicroBitWateringActuator.h"

#define UPDATE_TIMEOUT                      2000

#define WATERING_EVENT_ID                   55536 // Reserved events: [1-65535]
#define WATERING_EVENT_REQUESTED            1
#define WATERING_EVENT_STARTED              2
#define WATERING_EVENT_ENDED                3
#define WATERING_TIMEOUT                    5000

// uBit Services
MicroBit uBit;
MicroBitLightService *lightService;
MicroBitTemperatureService *temperatureService;
MicroBitMoistureService *moistureService;

// Sensors
MicroBitMoistureSensor moistureSensor(uBit.io.P0, uBit.io.P1);
MicroBitWateringActuator wateringActuator(uBit.io.P2);

// Data
/**
 * Preset moisture level in percentage.
 * TODO: this will be changed with a configuration-defined value
 */
int32_t targetMoisture = 20;

/**
 * If true the watering operation is forced to run
 */
bool forceWatering = false;

/**
 * A listener to perform actions after a BLE device connects.
 */
void onConnected(MicroBitEvent)
{
    uBit.display.scroll("C");
}

/**
 * A listener to perform actions after a BLE device disconnects.
 */
void onDisconnected(MicroBitEvent)
{
    uBit.display.scroll("D");
}

/**
 * Return true if watering is needed.
 */
bool canWater()
{
    // No watering if the vase is actually watering the plant
    if(wateringActuator.isWatering())
        return false;

    if(forceWatering)
        return true;
    
    // Check moisture level for watering
    int32_t moisture = moistureSensor.getMoistureLevel();

    return moisture < targetMoisture;
}

/**
 * Initialise the micro:bit and its sensors.
 */
void init()
{
    // Initialise the micro:bit runtime.
    uBit.init();

    // Calibrate termometer
    uBit.thermometer.setCalibration(uBit.thermometer.getTemperature());

    // This will returns 0 on the first call
    uBit.display.readLightLevel();
}

/**
 * Fiber code to check sensor values.
 */
void updateFiber()
{
    while (1)
    {
        // Check for watering
        if (canWater())
        {
            MicroBitEvent evt = MicroBitEvent(WATERING_EVENT_ID, WATERING_EVENT_REQUESTED);
            evt.fire();
        }

        // Sleep a bit before update again
        uBit.sleep(UPDATE_TIMEOUT);
    }
}

/**
 * Handles watering.
 */
void wateringFiber()
{
    while (1)
    {
        // Wait for the watering event
        fiber_wait_for_event(WATERING_EVENT_ID, WATERING_EVENT_REQUESTED);

        wateringActuator.startWatering();

        uBit.sleep(WATERING_TIMEOUT);

        wateringActuator.stopWatering();

        // disable forcing watering after watering
        forceWatering = false;
    }
}

/**
 * Handler for Button A click.
 */
void onButtonAPressed(MicroBitEvent)
{
    forceWatering = true;
}

int main()
{
    init();

    // BLE setup
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButtonAPressed);

    lightService = new MicroBitLightService(*uBit.ble, uBit.display);
    temperatureService = new MicroBitTemperatureService(*uBit.ble, uBit.thermometer);
    moistureService = new MicroBitMoistureService(*uBit.ble, moistureSensor);

    // Setup fibers
    create_fiber(updateFiber);
    create_fiber(wateringFiber);

    uBit.display.scroll("READY");

    release_fiber();
}
