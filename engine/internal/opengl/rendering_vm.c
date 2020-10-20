#include "engine/api/code.h"
#include "engine/api/rendering_vm.h"
#include "opengl.h"

#define GET_VALUE(type, name) type name; memcpy(&name, buffer, sizeof(name)); buffer += sizeof(name);
#define OGL_VERSION(major, minor) (major * 10 + minor)

#define REGISTRY_RVM_INSTRUCTION(name) static void impl_ ## name(u8 const * buffer);
#include "engine/registry/rendering_vm_instruction.h"

//
// API
//

struct Rendering_VM {
	GLint version;
};
static struct Rendering_VM * rvm;

void engine_rendering_vm_init(void) {
	struct Rendering_VM * rendering_vm = ENGINE_MALLOC(sizeof(*rvm));

	GLint version_major, version_minor;
	glGetIntegerv(GL_MAJOR_VERSION, &version_major);
	glGetIntegerv(GL_MINOR_VERSION, &version_minor);
	rendering_vm->version = OGL_VERSION(version_major, version_minor);

	rvm = rendering_vm;
}

void engine_rendering_vm_deinit(void) {
	ENGINE_FREE(rvm);
}

void engine_rendering_vm_update(u8 const * buffer, size_t buffer_length) {
	u8 const * buffer_end = buffer + buffer_length;
	while (buffer < buffer_end) {
		GET_VALUE(enum RVM_Instruction, instruction)
		switch (instruction) {
			#define REGISTRY_RVM_INSTRUCTION(name) case RVM_ ## name: impl_ ## name(buffer); break;
			#include "engine/registry/rendering_vm_instruction.h"
		}
	}
}

//
// internal implementation
//

// mapping
static GLenum get_comparison(enum RVM_Comparison value) {
	switch (value) {
		case RVM_Comparison_False:   return GL_NEVER;
		case RVM_Comparison_True:    return GL_ALWAYS;
		case RVM_Comparison_Less:    return GL_LESS;
		case RVM_Comparison_LEqual:  return GL_LEQUAL;
		case RVM_Comparison_Equal:   return GL_EQUAL;
		case RVM_Comparison_NEqual:  return GL_NOTEQUAL;
		case RVM_Comparison_Greater: return GL_GREATER;
		case RVM_Comparison_GEqual:  return GL_GEQUAL;
	}
	ENGINE_DEBUG_BREAK();
	return GL_NONE;
}

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
	GET_VALUE(bool, value)
	if (value) { glEnable(GL_DEPTH_TEST); }
	else       { glDisable(GL_DEPTH_TEST); }
}

static void impl_Depth_Set_Write(u8 const * buffer) {
	GET_VALUE(bool, value)
	glDepthMask(value);
}

static void impl_Depth_Set_Clear(u8 const * buffer) {
	GET_VALUE(r32, value)
	if (rvm->version >= OGL_VERSION(4, 1)) {
		glClearDepthf(value);
	}
	else {
		glClearDepth((double)value);
	}
}

static void impl_Depth_Set_Comparison(u8 const * buffer) {
	GET_VALUE(enum RVM_Comparison, value)
	glDepthFunc(get_comparison(value));
}

static void impl_Depth_Set_Range(u8 const * buffer) {
	GET_VALUE(vec2, value)
	if (rvm->version >= OGL_VERSION(4, 1)) {
		glDepthRangef(value.x, value.y);
	}
	else {
		glDepthRange((double)value.x, (double)value.y);
	}
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

#undef GET_VALUE
