#include "engine/api/code.h"
#include "engine/api/types.h"
#include "engine/api/file.h"
#include "engine/api/dummy.h"

int main(int argc, char * argv[]) {
	(void)argc; (void)argv;
	dummy();

	u8 * buffer = NULL; size_t buffer_size = 0;
	engine_file_read("assets/settings.cfg", &buffer, &buffer_size);
	printf("%s\n", buffer);

	printf("file time: %zd\n", engine_file_time("assets/settings.cfg"));

	free(buffer);

	getchar();
}
