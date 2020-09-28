#if !defined(ENGINE_CODE)
#define ENGINE_CODE

#include <stdlib.h>
#include <stdio.h>
#include <intrin.h>

#if !defined(ENGINE_MALLOC)
#define ENGINE_MALLOC(size)         malloc(size)
#define ENGINE_REALLOC(block, size) realloc(block, size)
#define ENGINE_FREE(block)          free(block)
#elif !defined(ENGINE_REALLOC) || !defined(ENGINE_FREE)
#error "define all of the ENGINE_MALLOC, ENGINE_REALLOC, ENGINE_FREE"
#endif // ENGINE_MALLOC

#if defined(_MSC_VER)
#define ENGINE_DEBUG_BREAK() __debugbreak()
#else
#define ENGINE_DEBUG_BREAK() (void)0
#endif // _MSC_VER

#endif // ENGINE_CODE
