function octave_example_callback()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your Compass Bricklet

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    c = javaObject("com.tinkerforge.BrickletCompass", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register heading callback to function cb_heading
    c.addHeadingCallback(@cb_heading);

    % Set period for heading callback to 0.1s (100ms) without a threshold
    c.setHeadingCallbackConfiguration(100, false, "x", 0, 0);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for heading callback
function cb_heading(e)
    fprintf("Heading: %g °\n", e.heading/10.0);
end
