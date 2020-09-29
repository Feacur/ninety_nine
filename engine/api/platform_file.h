#if !defined(ENGINE_FILE)
#define ENGINE_FILE

#include "engine/api/types.h"

u64    engine_file_time(cstring path);
size_t engine_file_read(cstring path, u8 ** buffer, size_t * size);

#endif // ENGINE_FILE
