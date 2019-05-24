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

#include "MicroBitConfig.h"
#include "MicroBitMoistureSensor.h"
#include "MicroBitSystemTimer.h"
#include "MicroBitFiber.h"

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
MicroBitMoistureSensor::MicroBitMoistureSensor(MicroBitPin &_readPin, MicroBitPin &_writePin, uint16_t id) : 
readPin(&_readPin), writePin(&_writePin)
{
    this->id = id;
    this->samplePeriod = MICROBIT_MOISTURE_PERIOD;
    this->sampleTime = 0;
}

/**
  * Gets the current moisture level read by the microbit.
  *
  * @return the current moisture.
  *
  * @code
  * moistureSensor.getMoistureLevel();
  * @endcode
  */
int MicroBitMoistureSensor::getMoistureLevel()
{
    updateSample();
    return moisture;
}


/**
  * Updates the moisture sample of this instance of MicroBitMoistureSensor
  * only if isSampleNeeded() indicates that an update is required.
  *
  * This call also will add the thermometer to fiber components to receive
  * periodic callbacks.
  *
  * @return MICROBIT_OK on success.
  */
int MicroBitMoistureSensor::updateSample()
{
    if(!(status & MICROBIT_MOISTURE_ADDED_TO_IDLE))
    {
        // If we're running under a fiber scheduer, register ourselves for a periodic callback to keep our data up to date.
        // Otherwise, we do just do this on demand, when polled through our read() interface.
        fiber_add_idle_component(this);
        status |= MICROBIT_MOISTURE_ADDED_TO_IDLE;
    }

    // check if we need to update our sample...
    if(isSampleNeeded())
    {
        // Read moisture value
        writePin->setAnalogValue(1023);
        moisture = readPin->getAnalogValue();
        writePin->setAnalogValue(0);

        // Schedule our next sample.
        sampleTime = system_timer_current_time() + samplePeriod;

        // Send an event to indicate that we'e updated our moisture.
        MicroBitEvent e(id, MICROBIT_MOISTURE_EVT_UPDATE);
    }

    return MICROBIT_OK;
};

/**
  * Periodic callback from MicroBit idle thread.
  */
void MicroBitMoistureSensor::idleTick()
{
    updateSample();
}

/**
  * Determines if we're due to take another moisture reading
  *
  * @return 1 if we're due to take a moisture reading, 0 otherwise.
  */
int MicroBitMoistureSensor::isSampleNeeded()
{
    return  system_timer_current_time() >= sampleTime;
}

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
void MicroBitMoistureSensor::setPeriod(int period)
{
    updateSample();
    samplePeriod = period;
}

/**
  * Reads the currently configured sample rate of the sensor.
  *
  * @return The time between samples, in milliseconds.
  */
int MicroBitMoistureSensor::getPeriod()
{
    return samplePeriod;
}


/**
 * Set the pin used by the sensor.
 */
void MicroBitMoistureSensor::setWritePin(MicroBitPin &pin)
{
    this->writePin = &pin;
    updateSample();
}

/**
 * Set the pin used by the sensor for reading the moisture level.
 */
void MicroBitMoistureSensor::setReadPin(MicroBitPin &pin)
{
    this->readPin = &pin;
    updateSample();
}
