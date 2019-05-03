#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_compass'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change XYZ to the UID of your Compass Bricklet

ipcon = IPConnection.new # Create IP connection
c = BrickletCompass.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Register heading callback
c.register_callback(BrickletCompass::CALLBACK_HEADING) do |heading|
  puts "Heading: #{heading/10.0} °"
end

# Set period for heading callback to 0.1s (100ms) without a threshold
c.set_heading_callback_configuration 100, false, 'x', 0, 0

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
