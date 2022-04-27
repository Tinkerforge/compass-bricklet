// This example is not self-contained.
// It requires usage of the example driver specific to your platform.
// See the HAL documentation.

#include "src/bindings/hal_common.h"
#include "src/bindings/bricklet_compass.h"

void check(int rc, const char *msg);
void example_setup(TF_HAL *hal);
void example_loop(TF_HAL *hal);

static TF_Compass c;

void example_setup(TF_HAL *hal) {
	// Create device object
	check(tf_compass_create(&c, NULL, hal), "create device object");

	// Get current heading
	int16_t heading;
	check(tf_compass_get_heading(&c, &heading), "get heading");

	tf_hal_printf("Heading: %d 1/%d °\n", heading, 10);
}

void example_loop(TF_HAL *hal) {
	// Poll for callbacks
	tf_hal_callback_tick(hal, 0);
}
