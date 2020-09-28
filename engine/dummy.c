#include "engine/code.h"
#include "engine/dummy.h"

static void dummy_internal(void) {
	printf("Hello, World!\n");
}

void dummy(void) {
	dummy_internal();
}
