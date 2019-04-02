let moisture = 0
let temperature = 0
let light = 0

input.onButtonPressed(Button.A, function () {
    log()
})

function showValues() {
    basic.showString("LGT: " + light)
    basic.showString("TMP: " + temperature)
    basic.showString("MST: " + moisture)
}

function updateMeasurements() {
    light = input.lightLevel()
    temperature = input.temperature()
    moisture = pins.digitalReadPin(DigitalPin.P0)
}

function log() {
    serial.writeString("{")
    serial.writeLine("\"temperature\": " + temperature + ",")
    serial.writeLine("\"light\": " + light + ",")
    serial.writeLine("\"moisture\": " + moisture + "")
    serial.writeLine("}")
}

basic.forever(function () {
    updateMeasurements()
    basic.pause(5000)
})

