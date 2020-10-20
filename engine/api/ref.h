#if !defined(ENGINE_REF)
#define ENGINE_REF

#include "engine/api/types.h"

struct Ref {
	u32 id, gen;
};

#define REF_EMPTY_ID UINT32_MAX

#endif // ENGINE_REF
