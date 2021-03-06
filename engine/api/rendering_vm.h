#if !defined(ENGINE_RENDERING_VM)
#define ENGINE_RENDERING_VM

#include "engine/api/primitive_types.h"

void engine_rendering_vm_init(void);
void engine_rendering_vm_deinit(void);
void engine_rendering_vm_update(u8 const * buffer, size_t buffer_length);

enum RVM_Instruction {
	#define REGISTRY_RVM_INSTRUCTION(name) RVM_Instruction_ ## name,
	#include "engine/registry/rendering_vm_instruction.h"
};

enum RVM_Comparison {
	RVM_Comparison_False,   RVM_Comparison_True,
	RVM_Comparison_Less,    RVM_Comparison_LEqual,
	RVM_Comparison_Equal,   RVM_Comparison_NEqual,
	RVM_Comparison_Greater, RVM_Comparison_GEqual,
};

enum RVM_Operation {
	RVM_Operation_Keep, RVM_Operation_Invert,
	RVM_Operation_Zero, RVM_Operation_Replace,
	RVM_Operation_Incr, RVM_Operation_Incr_Wrap,
	RVM_Operation_Decr, RVM_Operation_Decr_Wrap,
};

enum RVM_Color_Write {
	RVM_Color_Write_None = 0,
	RVM_Color_Write_R = (1 << 0),
	RVM_Color_Write_G = (1 << 1),
	RVM_Color_Write_B = (1 << 2),
	RVM_Color_Write_A = (1 << 3),
};

enum RVM_Color_Blend {
	RVM_Color_Blend_Opaque,
	RVM_Color_Blend_Alpha,
	RVM_Color_Blend_Additive,
	RVM_Color_Blend_Multiply,
	RVM_Color_Blend_PMAlpha,
	RVM_Color_Blend_PMAdditive,
};

enum RVM_Face_Cull {
	RVM_Face_Cull_None,
	RVM_Face_Cull_Back,
	RVM_Face_Cull_Front,
	RVM_Face_Cull_Both,
};

enum RVM_Face_Front {
	RVM_Face_Front_CCW,
	RVM_Face_Front_CW,
};

#endif // ENGINE_RENDERING_VM
