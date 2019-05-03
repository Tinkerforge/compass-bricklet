#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Compass Bricklet

# Get current heading
tinkerforge call compass-bricklet $uid get-heading
