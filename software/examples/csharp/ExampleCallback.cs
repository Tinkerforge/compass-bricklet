using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your Compass Bricklet

	// Callback function for heading callback
	static void HeadingCB(BrickletCompass sender, short heading)
	{
		Console.WriteLine("Heading: " + heading/10.0 + " °");
	}

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletCompass c = new BrickletCompass(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Register heading callback to function HeadingCB
		c.HeadingCallback += HeadingCB;

		// Set period for heading callback to 0.1s (100ms) without a threshold
		c.SetHeadingCallbackConfiguration(100, false, 'x', 0, 0);

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
