#include "MicroBit.h"

#include "MicroBitTemperatureService.h"
#include "services/light/MicroBitLightService.h"
#include "services/moisture/MicroBitMoistureService.h"
#include "services/watering/MicroBitWateringService.h"

#include "sensors/moisture/MicroBitMoistureSensor.h"

#include "actuators/watering/MicroBitWateringActuator.h"

#define UPDATE_TIMEOUT 5000

#define WATERING_EVENT_ID 55536 // Reserved events: [1-65535]
#define WATERING_EVENT_REQUESTED 1
#define WATERING_EVENT_STARTED 2
#define WATERING_EVENT_ENDED 3
#define WATERING_TIMEOUT 5000

// uBit Services
MicroBit uBit;
MicroBitLightService *lightService;
MicroBitTemperatureService *temperatureService;
MicroBitMoistureService *moistureService;
MicroBitWateringService *wateringService;

// Sensors
MicroBitMoistureSensor moistureSensor(uBit.io.P0, uBit.io.P1);
MicroBitWateringActuator wateringActuator(uBit.io.P2);

// Images
MicroBitImage drop("0,0,255,0, 0\n0,0,255,0,0\n0,255,0,255,0\n255,0,0,0,255\n0,255,0,255,0\n");
MicroBitImage arrowDown("0,0,255,0, 0\n0,0,255,0,0\n255,0,255,0,255\n0,255,255,255,0\n0,0,255,0,0\n");

/**
 * If true the watering operation is forced to run
 */
bool forceWatering = false;

/**
 * A listener to perform actions after a BLE device connects.
 */
void onConnected(MicroBitEvent)
{
    uBit.display.scrollAsync('C');
    uBit.sleep(50);
}

/**
 * A listener to perform actions after a BLE device disconnects.
 */
void onDisconnected(MicroBitEvent)
{
    uBit.display.scrollAsync('D');
    uBit.sleep(50);
}

/**
 * Return true if watering is needed.
 */
bool canWater()
{
    // No watering if the vase is actually watering the plant or there isn't enough water
    if (wateringActuator.isWatering() || !wateringActuator.enoughWater())
    {
        return false;
    }

    if (forceWatering)
        return true;

    // Check moisture level for watering
    int32_t moisture = moistureSensor.getMoistureLevel();

    return moisture < (*moistureService).getMoistureLevelTreshold();
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
    uBit.display.setDisplayMode(DISPLAY_MODE_BLACK_AND_WHITE_LIGHT_SENSE);
}

/**
 * Start and stop the watering actuator.
 */
void performWatering()
{
    if (!wateringActuator.isWatering())
    {
        wateringActuator.startWatering();

        uBit.display.printAsync(drop);

        uBit.sleep(WATERING_TIMEOUT);

        wateringActuator.stopWatering();

        uBit.display.clear();

        // disable forcing watering after watering
        forceWatering = false;
    }
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
        performWatering();
    }
}

void onWateringRequested(MicroBitEvent)
{
    MicroBitEvent evt = MicroBitEvent(WATERING_EVENT_ID, WATERING_EVENT_REQUESTED);
    evt.fire();
}

/**
 * Handler for Button A click.
 */
void onButtonAPressed(MicroBitEvent)
{
    forceWatering = true;
}

void onButtonBPressed(MicroBitEvent)
{
    wateringActuator.setWateringCount(MICROBIT_WATERINGS_COUNT);
    uBit.display.scrollAsync(wateringActuator.getWateringCount());
}

void onMoistureUpdated(MicroBitEvent)
{
    int t = (int)(*moistureService).getMoistureLevelTreshold();
    uBit.display.scrollAsync(t);
}

int main()
{
    init();

    // BLE setup
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, onButtonAPressed);
    uBit.messageBus.listen(MICROBIT_ID_WATERING_SERVICE, WATERING_EVT_REQUESTED, onWateringRequested);
    uBit.messageBus.listen(MICROBIT_ID_MOISTURE_SERVICE, MOISTURE_TRESHOLD_UPDATED, onMoistureUpdated);

    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, onButtonBPressed);

    lightService = new MicroBitLightService(*uBit.ble, uBit.display);
    temperatureService = new MicroBitTemperatureService(*uBit.ble, uBit.thermometer);
    moistureService = new MicroBitMoistureService(*uBit.ble, moistureSensor);
    wateringService = new MicroBitWateringService(*uBit.ble, wateringActuator);

    // Setup fibers
    create_fiber(updateFiber);
    create_fiber(wateringFiber);

    release_fiber();
}