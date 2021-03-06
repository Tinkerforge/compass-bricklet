package main

import (
	"fmt"
	"github.com/Tinkerforge/go-api-bindings/compass_bricklet"
	"github.com/Tinkerforge/go-api-bindings/ipconnection"
)

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your Compass Bricklet.

func main() {
	ipcon := ipconnection.New()
	defer ipcon.Close()
	c, _ := compass_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
	defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	c.RegisterHeadingCallback(func(heading int16) {
		fmt.Printf("Heading: %f °\n", float64(heading)/10.0)
	})

	// Set period for heading callback to 0.1s (100ms) without a threshold.
	c.SetHeadingCallbackConfiguration(100, false, 'x', 0, 0)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()
}
