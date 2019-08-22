use std::{error::Error, io, thread};
use tinkerforge::{compass_bricklet::*, ip_connection::IpConnection};

const HOST: &str = "localhost";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Compass Bricklet.

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection.
    let c = CompassBricklet::new(UID, &ipcon); // Create device object.

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd.
                                          // Don't use device before ipcon is connected.

    let heading_receiver = c.get_heading_callback_receiver();

    // Spawn thread to handle received callback messages.
    // This thread ends when the `c` object
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for heading in heading_receiver {
            println!("Heading: {} °", heading as f32 / 10.0);
        }
    });

    // Set period for heading callback to 0.1s (100ms) without a threshold.
    c.set_heading_callback_configuration(100, false, 'x', 0, 0);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
