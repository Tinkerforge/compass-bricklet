function matlab_example_callback()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletCompass;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your Compass Bricklet

    ipcon = IPConnection(); % Create IP connection
    c = handle(BrickletCompass(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register heading callback to function cb_heading
    set(c, 'HeadingCallback', @(h, e) cb_heading(e));

    % Set period for heading callback to 0.1s (100ms) without a threshold
    c.setHeadingCallbackConfiguration(100, false, 'x', 0, 0);

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for heading callback
function cb_heading(e)
    fprintf('Heading: %g °\n', e.heading/10.0);
end
