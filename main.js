let moisture = 0
let temperature = 0
let connected = false
let light = 0
input.onButtonPressed(Button.A, function () {
    log()
})
function displayBTStatus() {
    if (connected) {
        basic.showString("C")
    } else {
        basic.showString("D")
    }
}
function updateMeasurements() {
    light = input.lightLevel()
    temperature = input.temperature()
    pins.analogWritePin(AnalogPin.P1, 1023)
    moisture = pins.analogReadPin(AnalogPin.P0)
    pins.analogWritePin(AnalogPin.P1, 0)
}
function sendRegisterRequest() {
    serial.writeLine("Sending registration request")
    bluetooth.uartWriteString("whoiam:vase")
}
bluetooth.onBluetoothDisconnected(function () {
    serial.writeLine("Disconnected!")
    connected = false
    registered = false
    displayBTStatus()
})
function log() {
    serial.writeLine("{")
    serial.writeLine("\"temperature\": " + temperature + ",")
    serial.writeLine("\"light\": " + light + ",")
    serial.writeLine("\"moisture\": " + moisture + "")
    serial.writeLine("}")
}
function sendDataWithUART() {
    bluetooth.uartWriteValue("temperature", temperature)
    bluetooth.uartWriteValue("light", light)
    bluetooth.uartWriteValue("moisture", moisture)
}
bluetooth.onBluetoothConnected(function () {
    serial.writeLine("Connected!")
    connected = true
    displayBTStatus()
})
bluetooth.onUartDataReceived(serial.delimiters(Delimiters.NewLine), function () {
    uart_data = bluetooth.uartReadUntil(serial.delimiters(Delimiters.NewLine))
    if (!(registered)) {
        switch (uart_data) {
            case "whoareyou":
                sendRegisterRequest()
                break;
            case "OK":
                registered = true
                serial.writeLine("Registered with the fog node")
                break;
            default:
                serial.writeLine("Something wrong happened: " + uart_data)
                break;
        }
    } else {
        serial.writeString("Data received: " + uart_data)
    }
})
let uart_data = ""
let registered = false
bluetooth.startUartService()
basic.forever(function () {
    displayBTStatus()
    if (connected && registered) {
        updateMeasurements()
        serial.writeLine("Sending data using UART")
        sendDataWithUART()
    } else {
        serial.writeLine("Not connected nor registered...")
    }
    basic.pause(2000)
})
