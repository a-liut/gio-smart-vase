
#ifndef MICROBIT_WATERING_ACTUATOR_H
#define MICROBIT_WATERING_ACTUATOR_H

#include "MicroBitPin.h"

#define MICROBIT_ID_WATERING_ACTUATOR                   1235
#define MICROBIT_WATERING_ACTUATOR_EVT_UPDATE           10

#define MICROBIT_WATERING_ON                    1
#define MICROBIT_WATERING_OFF                   0

/**
 * Class definition for the custom MicroBit WateringActuator.
 * Manages the watering of the plant.
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
     * Start watering immediately
     */
    void startWatering();

    /**
     * Stops watering immediately
     */
    void stopWatering();

    /**
     * Return MICROBIT_WATERING_ON if the actuator is enabled, otherwise MICROBIT_WATERING_OFF.
     */
    bool isWatering();

    private:

    /**
     * Start the pump
     */
    void startPump();

    /**
     * Stop the pump
     */
    void stopPump();

    // Pin to use to start/stop the pump
    MicroBitPin &trigger;

    // Actuator's current status
    int32_t status;
};

#endif