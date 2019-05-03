#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Compass Bricklet

# Handle incoming heading callbacks
tinkerforge dispatch compass-bricklet $uid heading &

# Set period for heading callback to 0.1s (100ms) without a threshold
tinkerforge call compass-bricklet $uid set-heading-callback-configuration 100 false threshold-option-off 0 0

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
