#if !defined(ENGINE_RENDERING_VM)
#define ENGINE_RENDERING_VM

#include "engine/api/types.h"

enum RVM_Instruction {
	#define REGISTRY_RVM_INSTRUCTION(name) RVM_ ## name,
	#include "engine/registry/rendering_vm_instruction.h"
};

void engine_rendering_vm_update(u8 const * buffer, size_t buffer_length);

#endif // ENGINE_RENDERING_VM
