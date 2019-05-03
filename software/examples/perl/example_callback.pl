#!/usr/bin/perl

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletCompass;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your Compass Bricklet

# Callback subroutine for heading callback
sub cb_heading
{
    my ($heading) = @_;

    print "Heading: " . $heading/10.0 . " °\n";
}

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $c = Tinkerforge::BrickletCompass->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Register heading callback to subroutine cb_heading
$c->register_callback($c->CALLBACK_HEADING, 'cb_heading');

# Set period for heading callback to 0.1s (100ms) without a threshold
$c->set_heading_callback_configuration(100, 0, 'x', 0, 0);

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
