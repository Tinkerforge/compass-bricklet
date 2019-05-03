function octave_example_simple()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your Compass Bricklet

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    c = javaObject("com.tinkerforge.BrickletCompass", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get current heading
    heading = c.getHeading();
    fprintf("Heading: %g °\n", heading/10.0);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end
