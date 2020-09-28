#include "engine/code.h"
#include "engine/types.h"
#include "engine/dummy.h"

int main(int argc, char * argv[]) {
	(void)argc; (void)argv;
	dummy();

	FILE * file = fopen("assets/settings.cfg", "rb");
	fseek(file, 0, SEEK_END);
	size_t file_size = (size_t)ftell(file);
	fseek(file, 0, SEEK_SET);
	u8 * buffer = malloc((size_t)file_size + 1);
	size_t bytes_read = fread(buffer, sizeof(u8), file_size, file);
	fclose(file);

	printf("file size %zd; bytes read %zd\n", file_size, bytes_read);

	buffer[file_size] = '\0';
	printf("%s\n", buffer);

	free(buffer);

	getchar();
}
