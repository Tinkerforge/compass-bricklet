#include "bindings/hal_common.h"
#include "bindings/bricklet_compass.h"

#define UID "XYZ" // Change XYZ to the UID of your Compass Bricklet

void check(int rc, const char* msg);

// Callback function for heading callback
void heading_handler(TF_Compass *device, int16_t heading, void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

	tf_hal_printf("Heading: %d 1/%d °\n", heading, 10.0);
}

TF_Compass c;

void example_setup(TF_HalContext *hal) {
	// Create device object
	check(tf_compass_create(&c, UID, hal), "create device object");

	// Register heading callback to function heading_handler
	tf_compass_register_heading_callback(&c,
	                                     heading_handler,
	                                     NULL);

	// Set period for heading callback to 0.1s (100ms) without a threshold
	tf_compass_set_heading_callback_configuration(&c, 100, false, 'x', 0, 0);
}

void example_loop(TF_HalContext *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
