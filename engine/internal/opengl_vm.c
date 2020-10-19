#include "engine/api/code.h"
#include "engine/api/rendering_vm.h"
#include "engine/api/rendering_vm_instruction.h"
#include "engine/api/opengl.h"

#define REGISTRY_RVM_INSTRUCTION(name) static void impl_ ## name(u8 const * buffer);
#include "engine/registry/rendering_vm_instruction.h"

void engine_rendering_vm_update(u8 const * buffer, size_t buffer_length) {
	u8 const * buffer_end = buffer + buffer_length;
	while (buffer < buffer_end) {
		enum RVM_Instruction instruction;
		if ((size_t)(buffer_end - buffer) < sizeof(instruction)) { ENGINE_DEBUG_BREAK(); break; }
		memcpy(&instruction, buffer, sizeof(instruction)); buffer += sizeof(instruction);

		switch (instruction) {
			#define REGISTRY_RVM_INSTRUCTION(name) case RVM_ ## name: impl_ ## name(buffer); break;
			#include "engine/registry/rendering_vm_instruction.h"
		}
	}
}

//
// internal implementation
//

// Common
static void impl_Common_Set_Clip(u8 const * buffer) {
	++buffer;
}

static void impl_Common_Set_Viewport(u8 const * buffer) {
	++buffer;
}

// Color
static void impl_Color_Set_Read(u8 const * buffer) {
	++buffer;
}

static void impl_Color_Set_Write(u8 const * buffer) {
	++buffer;
}

static void impl_Color_Set_Clear(u8 const * buffer) {
	++buffer;
}

static void impl_Color_Set_Blend(u8 const * buffer) {
	++buffer;
}

// Depth
static void impl_Depth_Set_Read(u8 const * buffer) {
	++buffer;
}

static void impl_Depth_Set_Write(u8 const * buffer) {
	++buffer;
}

static void impl_Depth_Set_Clear(u8 const * buffer) {
	++buffer;
}

static void impl_Depth_Set_Comparison(u8 const * buffer) {
	++buffer;
}

static void impl_Depth_Set_Range(u8 const * buffer) {
	++buffer;
}

// Stencil
static void impl_Stencil_Set_Read(u8 const * buffer) {
	++buffer;
}

static void impl_Stencil_Set_Write(u8 const * buffer) {
	++buffer;
}

static void impl_Stencil_Set_Clear(u8 const * buffer) {
	++buffer;
}

static void impl_Stencil_Set_Comparison(u8 const * buffer) {
	++buffer;
}

static void impl_Stencil_Set_Operation(u8 const * buffer) {
	++buffer;
}

static void impl_Stencil_Set_Mask(u8 const * buffer) {
	++buffer;
}

// Vertex
static void impl_Vertex_Set_Cull(u8 const * buffer) {
	++buffer;
}

static void impl_Vertex_Set_Front(u8 const * buffer) {
	++buffer;
}

// Shader
static void impl_Shader_Allocate(u8 const * buffer) {
	++buffer;
}

static void impl_Shader_Free(u8 const * buffer) {
	++buffer;
}

static void impl_Shader_Load(u8 const * buffer) {
	++buffer;
}

static void impl_Shader_Use(u8 const * buffer) {
	++buffer;
}

static void impl_Shader_Uniform(u8 const * buffer) {
	++buffer;
}

// Mesh
static void impl_Mesh_Allocate(u8 const * buffer) {
	++buffer;
}

static void impl_Mesh_Free(u8 const * buffer) {
	++buffer;
}

static void impl_Mesh_Load(u8 const * buffer) {
	++buffer;
}

static void impl_Mesh_Use(u8 const * buffer) {
	++buffer;
}

// Target
// static void impl_Target_Allocate(u8 const * buffer) { }
// static void impl_Target_Free(u8 const * buffer) { }
// static void impl_Target_Load(u8 const * buffer) { }
// static void impl_Target_Use(u8 const * buffer) { }
// static void impl_Target_Clear(u8 const * buffer) { }

// Texture
static void impl_Texture_Allocate(u8 const * buffer) {
	++buffer;
}

static void impl_Texture_Free(u8 const * buffer) {
	++buffer;
}

static void impl_Texture_Load(u8 const * buffer) {
	++buffer;
}

// Sampler
// static void impl_Sampler_Allocate(u8 const * buffer) { }
// static void impl_Sampler_Free(u8 const * buffer) { }

// Unit
static void impl_Unit_Allocate(u8 const * buffer) {
	++buffer;
}

static void impl_Unit_Free(u8 const * buffer) {
	++buffer;
}

// Render
static void impl_Render_Clear(u8 const * buffer) {
	++buffer;
}

static void impl_Render_Draw(u8 const * buffer) {
	++buffer;
}
