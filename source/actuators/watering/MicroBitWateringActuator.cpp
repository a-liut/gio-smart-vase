#include "MicroBitPin.h"
#include "MicroBitEvent.h"
#include "MicroBitWateringActuator.h"

/**
  * Constructor.
  * Create a representation of the MicrovitWateringActuator
  * @param _trigger The pin used to drive the watering
  */
MicroBitWateringActuator::MicroBitWateringActuator(MicroBitPin &_trigger) : 
    trigger(_trigger)
{
    remaining_waterings = MICROBIT_WATERINGS_COUNT;

    // Make sure that the pump is off when creating the actuator.
    stopPump();

    status = MICROBIT_WATERING_OFF;
}

/**
  * Start watering immediately.
  *
  * @return the state of the actuator after the operation.
  */
void MicroBitWateringActuator::startWatering()
{
    if(status == MICROBIT_WATERING_OFF && enoughWater())
    {
        startPump();
        status = MICROBIT_WATERING_ON;

        remaining_waterings--;

        // Trigger event
        MicroBitEvent ev = MicroBitEvent(MICROBIT_ID_WATERING_ACTUATOR, MICROBIT_WATERING_ACTUATOR_EVT_UPDATE);
        ev.fire();
    }
}

/**
  * Stops watering immediately.
  *
  * @return the state of the actuator after the operation.
  */
void MicroBitWateringActuator::stopWatering()
{
    if(status == MICROBIT_WATERING_ON && enoughWater())
    {
        stopPump();
        status = MICROBIT_WATERING_OFF;

        // Trigger event
        MicroBitEvent ev = MicroBitEvent(MICROBIT_ID_WATERING_ACTUATOR, MICROBIT_WATERING_ACTUATOR_EVT_UPDATE);
        ev.fire();
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

/**
 * Return true if there is enough water for watering.
 */
bool MicroBitWateringActuator::enoughWater()
{
    return remaining_waterings > 0;
}

/**
 * Set the count of how many times the pump should be activated
 */
void MicroBitWateringActuator::setWateringCount(int c)
{
    if (c < 0)
    {
        c = MICROBIT_WATERINGS_COUNT;
    }

    remaining_waterings = c;
}