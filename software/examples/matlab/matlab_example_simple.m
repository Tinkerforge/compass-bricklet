function matlab_example_simple()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletCompass;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your Compass Bricklet

    ipcon = IPConnection(); % Create IP connection
    c = handle(BrickletCompass(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get current heading
    heading = c.getHeading();
    fprintf('Heading: %g °\n', heading/10.0);

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end
