#if !defined(ENGINE_RENDERING_VM)
#define ENGINE_RENDERING_VM

#include "engine/api/types.h"

void engine_rendering_vm_init(void);
void engine_rendering_vm_deinit(void);
void engine_rendering_vm_update(u8 const * buffer, size_t buffer_length);

enum RVM_Instruction {
	#define REGISTRY_RVM_INSTRUCTION(name) RVM_ ## name,
	#include "engine/registry/rendering_vm_instruction.h"
};

enum RVM_Comparison {
	RVM_Comparison_False,   RVM_Comparison_True,
	RVM_Comparison_Less,    RVM_Comparison_LEqual,
	RVM_Comparison_Equal,   RVM_Comparison_NEqual,
	RVM_Comparison_Greater, RVM_Comparison_GEqual,
};

#endif // ENGINE_RENDERING_VM
