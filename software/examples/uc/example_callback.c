// This example is not self-contained.
// It requires usage of the example driver specific to your platform.
// See the HAL documentation.

#include "src/bindings/hal_common.h"
#include "src/bindings/bricklet_compass.h"

void check(int rc, const char *msg);
void example_setup(TF_HAL *hal);
void example_loop(TF_HAL *hal);

// Callback function for heading callback
static void heading_handler(TF_Compass *device, int16_t heading, void *user_data) {
	(void)device; (void)user_data; // avoid unused parameter warning

	tf_hal_printf("Heading: %d 1/%d °\n", heading, 10);
}

static TF_Compass c;

void example_setup(TF_HAL *hal) {
	// Create device object
	check(tf_compass_create(&c, NULL, hal), "create device object");

	// Register heading callback to function heading_handler
	tf_compass_register_heading_callback(&c,
	                                     heading_handler,
	                                     NULL);

	// Set period for heading callback to 0.1s (100ms) without a threshold
	tf_compass_set_heading_callback_configuration(&c, 100, false, 'x', 0, 0);
}

void example_loop(TF_HAL *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
