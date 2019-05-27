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

#define VALUE_LABEL_TEMPERATURE             ManagedString("temperature")
#define VALUE_LABEL_MOISTURE                ManagedString("moisture")
#define VALUE_LABEL_LIGHT                   ManagedString("light")

MicroBit uBit;
MicroBitLightService *lightService;
MicroBitTemperatureService *temperatureService;
MicroBitMoistureService *moistureService;

MicroBitMoistureSensor moistureSensor(uBit.io.P0, uBit.io.P1);
MicroBitWateringActuator wateringActuator(uBit.io.P2);

bool connected = 0;

/**
 * If true the watering operation is forced to run
 */
bool forceWatering = false;

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
    moisture = moistureSensor.getMoistureLevel();
}

void onConnected(MicroBitEvent)
{
    uBit.display.scroll("C");
    connected = true;
}

void onDisconnected(MicroBitEvent)
{
    uBit.display.scroll("D");
    connected = false;
}

bool canWater()
{
    return forceWatering || wateringActuator.isWatering(); //TODO: check watering for real
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
 * Handles data updates.
 */
void updateFiber()
{
    while (1)
    {
        updateMeasurements();

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

    uBit.display.scroll("Started");

    release_fiber();
}
