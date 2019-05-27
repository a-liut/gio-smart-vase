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

#ifndef MICROBIT_MOISTURE_SENSOR_H
#define MICROBIT_MOISTURE_SENSOR_H

#include "mbed.h"
#include "MicroBitConfig.h"
#include "MicroBitComponent.h"
#include "MicroBitStorage.h"
#include "MicroBitPin.h"

#define MICROBIT_ID_MOISTURE                1234

#define MICROBIT_MOISTURE_PERIOD             1000


/*
 * Temperature events
 */
#define MICROBIT_MOISTURE_EVT_UPDATE         1

#define MICROBIT_MOISTURE_ADDED_TO_IDLE      2

/**
  * Class definition for MicroBit Moisture Sensor.
  *
  * Infers and stores the moisture based on reading two MicroBitPin objects.
  *
  */
class MicroBitMoistureSensor : public MicroBitComponent
{
    unsigned long           sampleTime;
    uint32_t                samplePeriod;
    int32_t                 moisture;
    MicroBitPin*            readPin;
    MicroBitPin*            writePin;

    public:

    /**
      * Constructor.
      * Create new MicroBitMoistureSensor that gives an indication of the current moisture level.
      *
      * @param id the unique EventModel id of this component. Defaults to MICROBIT_ID_MOISTURE.
      *
      * @code
      * MicroBitMoistureSensor moistureSensor;
      * @endcode
      */
    MicroBitMoistureSensor(MicroBitPin &_readPin, MicroBitPin &_writePin, uint16_t id = MICROBIT_ID_MOISTURE);

    /**
      * Set the sample rate at which the moisture is read (in ms).
      *
      * The default sample period is 1 second.
      *
      * @param period the requested time between samples, in milliseconds.
      *
      * @note the moisture is always read in the background, and is only updated
      * when the processor is idle, or when the moisture is explicitly read.
      */
    void setPeriod(int period);

    /**
      * Reads the currently configured sample rate of the sensor.
      *
      * @return The time between samples, in milliseconds.
      */
    int getPeriod();

    /**
      * Gets the current moisture level read by the microbit.
      *
      * @return the current moisture.
      *
      * @code
      * moistureSensor.getMoistureLevel();
      * @endcode
      */
    int32_t getMoistureLevel();

    /**
      * Updates the moisture sample of this instance of MicroBitThermometer
      * only if isSampleNeeded() indicates that an update is required.
      *
      * This call also will add the thermometer to fiber components to receive
      * periodic callbacks.
      *
      * @return MICROBIT_OK on success.
      */
    int updateSample();

    /**
      * Periodic callback from MicroBit idle thread.
      */
    virtual void idleTick();

    /**
     * Set the pin used by the sensor.
     */
    void setWritePin(MicroBitPin &pin);

    /**
     * Set the pin used by the sensor for reading the moisture level.
     */
    void setReadPin(MicroBitPin &pin);

    private:

    /**
      * Determines if we're due to take another moisture reading
      *
      * @return 1 if we're due to take a moisture reading, 0 otherwise.
      */
    int isSampleNeeded();
};

#endif