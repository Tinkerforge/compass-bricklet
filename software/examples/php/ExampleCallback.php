<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletCompass.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletCompass;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Compass Bricklet

// Callback function for heading callback
function cb_heading($heading)
{
    echo "Heading: " . $heading/10.0 . " °\n";
}

$ipcon = new IPConnection(); // Create IP connection
$c = new BrickletCompass(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Register heading callback to function cb_heading
$c->registerCallback(BrickletCompass::CALLBACK_HEADING, 'cb_heading');

// Set period for heading callback to 0.1s (100ms) without a threshold
$c->setHeadingCallbackConfiguration(100, FALSE, 'x', 0, 0);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
