#if !defined(ENGINE_RVM_INSTRUCTION)
#define ENGINE_RVM_INSTRUCTION

enum RVM_Instruction {
	#define REGISTRY_RVM_INSTRUCTION(name) RVM_ ## name,
	#include "engine/registry/rendering_vm_instruction.h"
};

#endif // ENGINE_RVM_INSTRUCTION
