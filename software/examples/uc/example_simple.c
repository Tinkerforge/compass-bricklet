#include "bindings/hal_common.h"
#include "bindings/bricklet_compass.h"

#define UID "XYZ" // Change XYZ to the UID of your Compass Bricklet

void check(int rc, const char* msg);

TF_Compass c;

void example_setup(TF_HalContext *hal) {
	// Create device object
	check(tf_compass_create(&c, UID, hal), "create device object");

	// Get current heading
	int16_t heading;
	check(tf_compass_get_heading(&c, &heading), "get heading");

	tf_hal_printf("Heading: %d 1/%d °\n", heading, 10.0);
}

void example_loop(TF_HalContext *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
