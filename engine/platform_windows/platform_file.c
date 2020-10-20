#include "engine/api/code.h"

#include <Windows.h>

//
// API
//

#include "engine/api/platform_file.h"

u64 engine_file_time(cstring path) {
	WIN32_FIND_DATA find_file_data;
	HANDLE handle = FindFirstFileA(path, &find_file_data);
	if (handle == INVALID_HANDLE_VALUE) {
		printf("[err]: failed to find file: `%s`", path);
		return 0;
	}

	ULARGE_INTEGER large = {
		.LowPart  = find_file_data.ftLastWriteTime.dwLowDateTime,
		.HighPart = find_file_data.ftLastWriteTime.dwHighDateTime,
	};

	FindClose(handle);
	return large.QuadPart ? (u64)large.QuadPart : 1;
}

size_t engine_file_read(cstring path, u8 ** buffer, size_t * buffer_size) {
	DWORD const share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
	HANDLE handle = CreateFileA(path, GENERIC_READ, share_mode, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE) {
		printf("[err]: failed to open file: `%s`", path);
		return 0;
	}

	LARGE_INTEGER file_size;
	if (!GetFileSizeEx(handle, &file_size)) {
		printf("[err]: failed to get file size: `%s`", path);
		return 0;
	}

	if (file_size.QuadPart > UINT32_MAX) {
		printf("[err]: file size is too large: `%s`", path);
		return 0;
	}

	*buffer_size = (size_t)file_size.QuadPart;
	*buffer = ENGINE_MALLOC(*buffer_size);

	DWORD number_of_bytes_read;
	if (!ReadFile(handle, *buffer, (DWORD)file_size.QuadPart, &number_of_bytes_read, NULL)) {
		printf("[err]: failed to read file: `%s`", path);
		ENGINE_FREE(*buffer); *buffer = NULL; *buffer_size = 0;
		return 0;
	}

	return (size_t)number_of_bytes_read;
}
