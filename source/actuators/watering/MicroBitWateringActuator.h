
#ifndef MICROBIT_WATERING_ACTUATOR_H
#define MICROBIT_WATERING_ACTUATOR_H

#include "MicroBitPin.h"

#define MICROBIT_ID_WATERING_ACTUATOR                   1235
#define MICROBIT_WATERING_ACTUATOR_EVT_UPDATE           10

#define MICROBIT_WATERING_ON                    1
#define MICROBIT_WATERING_OFF                   0

#define MICROBIT_WATERINGS_COUNT                4

/**
 * Class definition for the custom MicroBit WateringActuator.
 * Manages the watering of the plant.
 * For safety purposes, if the water level is too low or is not available, the pump is not activated.
 */
class MicroBitWateringActuator
{
    public:

    /**
     * Constructor.
     * Create a representation of the MicrovitWateringActuator
     * @param _trigger The pin used to drive the watering
     */
    MicroBitWateringActuator(MicroBitPin &_trigger);

    /**
     * Start watering immediately.
     * 
     * @return returns the state of the actuator after the operation.
     */
    void startWatering();

    /**
     * Stop watering immediately.
     * 
     * @return returns the state of the actuator after the operation.
     */
    void stopWatering();

    /**
     * Return MICROBIT_WATERING_ON if the actuator is enabled, otherwise MICROBIT_WATERING_OFF.
     */
    bool isWatering();

    /**
     * Return true if there is enough water for watering.
     */
    bool enoughWater();

    /**
     * Set the count of how many times the pump should be activated
     */
    void setWateringCount(int c);

    private:

    /**
     * Start the pump.
     */
    void startPump();

    /**
     * Stop the pump.
     */
    void stopPump();

    /**
     * Pin to use to start/stop the pump
     */
    MicroBitPin &trigger;

    /**
     * Waterings remaining
     */
    int32_t remaining_waterings;

    /**
     * Actuator's current status
     */
    int32_t status;
};

#endif