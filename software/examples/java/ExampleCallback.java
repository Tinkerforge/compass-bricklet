import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletCompass;

public class ExampleCallback {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;

	// Change XYZ to the UID of your Compass Bricklet
	private static final String UID = "XYZ";

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletCompass c = new BrickletCompass(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Add heading listener
		c.addHeadingListener(new BrickletCompass.HeadingListener() {
			public void heading(int heading) {
				System.out.println("Heading: " + heading/10.0 + " °");
			}
		});

		// Set period for heading callback to 0.1s (100ms) without a threshold
		c.setHeadingCallbackConfiguration(100, false, 'x', 0, 0);

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
