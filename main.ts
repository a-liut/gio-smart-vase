enum STATUS { REGISTERING = 0, MONITORING = 1, COMMUNICATING = 2, WATERING = 3, WAITING = 4 }
let status = STATUS.MONITORING
let pump_timeout = 0
let conf_mois_min = 0
let temperature = 0
let moisture = 0
let light = 0
let uart_data = ""
let connection_requested = false
// Config - moisture
conf_mois_min = 50
// - pump
pump_timeout = 5000
function canConnect() {
    return status != STATUS.WATERING && status != STATUS.COMMUNICATING && status != STATUS.REGISTERING
}
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
// function printStatus() {
//     switch (status) {
//         case STATUS.REGISTERING:
//             serial.writeLine("R")
//             break;
//         case STATUS.MONITORING:
//             serial.writeLine("M")
//             break;
//         case STATUS.COMMUNICATING:
//             serial.writeLine("C")
//             break;
//         case STATUS.WATERING:
//             serial.writeLine("W")
//             break;
//         case STATUS.WAITING:
//             serial.writeLine("A")
//             break;
//     }
// }
function startPump() {
    pins.digitalWritePin(DigitalPin.P2, 1)
}
function stopPump() {
    pins.digitalWritePin(DigitalPin.P2, 0)
}
function sendRegisterRequest() {
    // serial.writeLine("Sending registration request")
    bluetooth.uartWriteString("whoiam:vase")
}
function sendDataWithUART() {
    bluetooth.uartWriteValue("temperature", temperature)
    bluetooth.uartWriteValue("light", light)
    bluetooth.uartWriteValue("moisture", moisture)
    bluetooth.uartWriteString("EOS")
}
bluetooth.onBluetoothConnected(function () {
    // serial.writeLine("A new device has connected")
    if (canConnect()) {
        status = STATUS.REGISTERING
    }
    connection_requested = !canConnect()
})
bluetooth.onBluetoothDisconnected(function () {
    // serial.writeLine("The device has diconnected")
    status = STATUS.MONITORING
    connection_requested = false
})
bluetooth.onUartDataReceived(serial.delimiters(Delimiters.Dollar), function () {
    uart_data = bluetooth.uartReadUntil(serial.delimiters(Delimiters.Dollar))
    // serial.writeLine("Status on data received:")
    printStatus()
    if (status == STATUS.REGISTERING || connection_requested) {
        switch (uart_data) {
            case "whoareyou":
                sendRegisterRequest()
                break;
            case "OK":
                status = STATUS.COMMUNICATING
                connection_requested = false
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
    // printStatus()
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

            if (connection_requested) {
                // A connection have the priority on other actions
                status = STATUS.REGISTERING
            } else {
                // Check if the plant needs water
                if (checkWatering()) {
                    status = STATUS.WATERING
                } else {
                    basic.pause(2000)
                }
            }
            break;
        case STATUS.COMMUNICATING:
            // serial.writeLine("Sending data!")
            sendDataWithUART()
            status = STATUS.WAITING
            basic.pause(2000)
            break;
        default:
            basic.pause(2000)
            break;
    }
})
