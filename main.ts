enum STATUS { REGISTERING = 0, MONITORING = 1, COMMUNICATING = 2, WATERING = 3, WAITING = 4 }
let status = STATUS.MONITORING
let pump_timeout = 0
let conf_mois_min = 0
let moisture = 0
let light = 0
let uart_data = ""
let temperature = 0
// Config - moisture
conf_mois_min = 50
// - pump
pump_timeout = 5000
function checkWatering() {
    return temperature < conf_mois_min
}
function updateMeasurements() {
    // Light
    light = input.lightLevel()
    // Temperature
    temperature = input.temperature()
    // Moisture
    pins.analogWritePin(AnalogPin.P1, 1023)
    moisture = pins.analogReadPin(AnalogPin.P0)
    pins.analogWritePin(AnalogPin.P1, 0)
}
function displayBTStatus() {
    switch (status) {
        case STATUS.REGISTERING:
            basic.showString("R")
            break;
        case STATUS.MONITORING:
            basic.showString("M")
            break;
        case STATUS.COMMUNICATING:
            basic.showString("C")
            break;
        case STATUS.WATERING:
            basic.showString("W")
            break;
        case STATUS.WAITING:
            basic.showString("A")
            break;
    }
}
function startPump() {
    pins.digitalWritePin(DigitalPin.P2, 1)
}
function stopPump() {
    pins.digitalWritePin(DigitalPin.P2, 0)
}
function sendRegisterRequest() {
    serial.writeLine("Sending registration request")
    bluetooth.uartWriteString("whoiam:vase")
}
function sendDataWithUART() {
    bluetooth.uartWriteValue("temperature", temperature)
    bluetooth.uartWriteValue("light", light)
    bluetooth.uartWriteValue("moisture", moisture)
    bluetooth.uartWriteString("EOS")
}
bluetooth.onBluetoothConnected(function () {
    displayBTStatus()
    status = STATUS.REGISTERING
})
bluetooth.onBluetoothDisconnected(function () {
    serial.writeLine("Disconnected!")
    status = STATUS.MONITORING
    displayBTStatus()
})
bluetooth.onUartDataReceived(serial.delimiters(Delimiters.Dollar), function () {
    uart_data = bluetooth.uartReadUntil(serial.delimiters(Delimiters.Dollar))
    if (status == STATUS.REGISTERING) {
        switch (uart_data) {
            case "whoareyou":
                sendRegisterRequest()
                break;
            case "OK":
                status = STATUS.COMMUNICATING
                break;
            default:
                serial.writeLine("Something wrong happened: " + uart_data)
                break;
        }
    } else {
        serial.writeString("Data received: " + uart_data)
    }
})
bluetooth.startUartService()
basic.forever(function () {
    displayBTStatus()
    switch (status) {
        case STATUS.WATERING:
            // Start the pump for watering
            startPump()
            basic.pause(pump_timeout)
            stopPump()

            status = STATUS.MONITORING
            break;
        case STATUS.MONITORING:
            updateMeasurements()

            // Check if the plant needs water
            if (checkWatering()) {
                status = STATUS.WATERING
            } else {
                basic.pause(2000)
            }
            break;
        case STATUS.COMMUNICATING:
            sendDataWithUART()
            status = STATUS.WAITING
            basic.pause(2000)
            break;
        default:
            basic.pause(2000)
            break;
    }
})
