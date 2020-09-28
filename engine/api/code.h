#if !defined(ENGINE_CODE)
#define ENGINE_CODE

#include <stdlib.h>
#include <stdio.h>

#if defined(_MSC_VER)
	#define ENGINE_DEBUG_BREAK() __debugbreak()
#else
	#define ENGINE_DEBUG_BREAK() (void)0
#endif

#endif // ENGINE_CODE
