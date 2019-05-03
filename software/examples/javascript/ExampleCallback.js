var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change XYZ to the UID of your Compass Bricklet

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var c = new Tinkerforge.BrickletCompass(UID, ipcon); // Create device object

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Set period for heading callback to 0.1s (100ms) without a threshold
        c.setHeadingCallbackConfiguration(100, false, 'x', 0, 0);
    }
);

// Register heading callback
c.on(Tinkerforge.BrickletCompass.CALLBACK_HEADING,
    // Callback function for heading callback
    function (heading) {
        console.log('Heading: ' + heading/10.0 + ' °');
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
