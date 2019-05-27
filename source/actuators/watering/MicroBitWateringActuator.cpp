#include "MicroBitPin.h"
#include "MicroBitWateringActuator.h"

/**
  * Constructor.
  * Create a representation of the MicrovitWateringActuator
  * @param _trigger The pin used to drive the watering
  */
MicroBitWateringActuator::MicroBitWateringActuator(MicroBitPin &_trigger) : trigger(_trigger)
{
    // Make sure that the pump is off when creating the actuator.
    stopPump();

    status = MICROBIT_WATERING_OFF;
}

/**
  * Start watering immediately
  */
void MicroBitWateringActuator::startWatering()
{
    if(status == MICROBIT_WATERING_OFF)
    {
        startPump();
        status = MICROBIT_WATERING_ON;
    }
}

/**
  * Stops watering immediately
  */
void MicroBitWateringActuator::stopWatering()
{
    if(status == MICROBIT_WATERING_ON)
    {
        stopPump();
        status = MICROBIT_WATERING_OFF;
    }
}

/**
 * Start the pump
 */
void MicroBitWateringActuator::startPump()
{
    trigger.setDigitalValue(1);
}

/**
 * Stop the pump
 */
void MicroBitWateringActuator::stopPump()
{
    trigger.setDigitalValue(0);
}

/**
 * Return MICROBIT_WATERING_ON if the actuator is enabled, otherwise MICROBIT_WATERING_OFF.
 */
bool MicroBitWateringActuator::isWatering()
{
    return status;
}