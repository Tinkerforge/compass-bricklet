<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletCompass.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletCompass;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Compass Bricklet

$ipcon = new IPConnection(); // Create IP connection
$c = new BrickletCompass(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Get current heading
$heading = $c->getHeading();
echo "Heading: " . $heading/10.0 . " °\n";

echo "Press key to exit\n";
fgetc(fopen('php://stdin', 'r'));
$ipcon->disconnect();

?>
