#include "engine/api/code.h"
#include "engine/api/dummy.h"

static void dummy_internal(void) {
	printf("Hello, World!\n");
}

void dummy(void) {
	dummy_internal();
}
