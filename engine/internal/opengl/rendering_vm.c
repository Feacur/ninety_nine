#include "engine/api/code.h"
#include "engine/api/ref.h"
#include "engine/api/math_types.h"
#include "engine/api/asset_types.h"
#include "engine/api/graphics_types.h"
#include "engine/api/rendering_vm.h"
#include "opengl.h"

#define GET_VALUE(type, name) type name; memcpy(&name, buffer, sizeof(name)); buffer += sizeof(name);
#define OGL_VERSION(major, minor) (major * 10 + minor)

#define REGISTRY_RVM_INSTRUCTION(name) static void impl_ ## name(u8 const * buffer);
#include "engine/registry/rendering_vm_instruction.h"

//
// API
//

struct VM_Shader;
struct VM_Mesh;
struct VM_Texture;
// struct VM_Sampler;
// struct VM_Target;
struct Rendering_VM {
	GLint version;
	//
	struct VM_Shader  * shaders;  size_t shaders_capacity;
	struct VM_Mesh    * meshes;   size_t meshes_capacity;
	struct VM_Texture * textures; size_t textures_capacity;
	// struct VM_Sampler * samplers; size_t sampler_capacity;
	// struct VM_Target  * targets;  size_t targets_capacity;
	//
	u32 shader, mesh, texture;
};
static struct Rendering_VM * rvm;

void engine_rendering_vm_init(void) {
	struct Rendering_VM * rendering_vm = ENGINE_MALLOC(sizeof(*rvm));

	GLint version_major, version_minor;
	glGetIntegerv(GL_MAJOR_VERSION, &version_major);
	glGetIntegerv(GL_MINOR_VERSION, &version_minor);
	rendering_vm->version = OGL_VERSION(version_major, version_minor);

	rendering_vm->shader  = REF_EMPTY_ID;
	rendering_vm->mesh    = REF_EMPTY_ID;
	rendering_vm->texture = REF_EMPTY_ID;

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
			#define REGISTRY_RVM_INSTRUCTION(name) case RVM_Instruction_ ## name: impl_ ## name(buffer); break;
			#include "engine/registry/rendering_vm_instruction.h"
		}
	}
}

//
// internal implementation
//

struct VM_Shader_Field {
	GLint location;
	u32 id;
};
struct VM_Shader {
	GLuint id;
	// struct VM_Shader_Field attributes[10]; size_t attributes_count;
	struct VM_Shader_Field uniforms[10];   size_t uniforms_count;
};

struct VM_Mesh {
	GLuint id;
};

struct VM_Texture {
	GLuint id;
};

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

static GLenum get_operation(enum RVM_Operation value) {
	switch (value) {
		case RVM_Operation_Keep:      return GL_KEEP;
		case RVM_Operation_Invert:    return GL_INVERT;
		case RVM_Operation_Zero:      return GL_ZERO;
		case RVM_Operation_Replace:   return GL_REPLACE;
		case RVM_Operation_Incr:      return GL_INCR;
		case RVM_Operation_Incr_Wrap: return GL_INCR_WRAP;
		case RVM_Operation_Decr:      return GL_DECR;
		case RVM_Operation_Decr_Wrap: return GL_DECR_WRAP;
	}
	ENGINE_DEBUG_BREAK();
	return GL_NONE;
}

static GLenum get_face_cull(enum RVM_Face_Cull value) {
	switch (value) {
		case RVM_Face_Cull_None:  return GL_NONE;
		case RVM_Face_Cull_Back:  return GL_BACK;
		case RVM_Face_Cull_Front: return GL_FRONT;
		case RVM_Face_Cull_Both:  return GL_FRONT_AND_BACK;
	}
	ENGINE_DEBUG_BREAK();
	return GL_NONE;
}

static GLenum get_face_front(enum RVM_Face_Front value) {
	switch (value) {
		case RVM_Face_Front_CCW: return GL_CCW;
		case RVM_Face_Front_CW:  return GL_CW;
	}
	ENGINE_DEBUG_BREAK();
	return GL_NONE;
}

static GLenum get_filter_min(enum Filter_Type mipmap, enum Filter_Type type) {
	switch (mipmap) {
		case Filter_Type_None: switch (type) { // no mipmaps
			case Filter_Type_None: return GL_NEAREST;
			case Filter_Type_Point: return GL_POINT;
			case Filter_Type_Linear: return GL_LINEAR;
		} break;
		case Filter_Type_Point: switch (type) { // single mipmap
			case Filter_Type_None: return GL_NEAREST_MIPMAP_NEAREST;
			case Filter_Type_Point: return GL_NEAREST_MIPMAP_NEAREST;
			case Filter_Type_Linear: return GL_LINEAR_MIPMAP_NEAREST;
		} break;
		case Filter_Type_Linear: switch (type) { // interpolate mipmaps
			case Filter_Type_None: return GL_NEAREST_MIPMAP_LINEAR;
			case Filter_Type_Point: return GL_NEAREST_MIPMAP_LINEAR;
			case Filter_Type_Linear: return GL_LINEAR_MIPMAP_LINEAR;
		} break;
	}
	ENGINE_DEBUG_BREAK();
	return GL_NONE;
}

static GLenum get_filter_max(enum Filter_Type type) {
	switch (type) {
		case Filter_Type_None: return GL_NEAREST;
		case Filter_Type_Point: return GL_NEAREST;
		case Filter_Type_Linear: return GL_LINEAR;
	}
	ENGINE_DEBUG_BREAK();
	return GL_NONE;
}

static GLenum get_wrap_type(enum Wrap_Type type) {
	switch (type) {
		case Wrap_Type_Clamp: return GL_CLAMP_TO_EDGE;
		case Wrap_Type_Repeat: return GL_REPEAT;
		case Wrap_Type_MClamp: return GL_MIRROR_CLAMP_TO_EDGE;
		case Wrap_Type_MRepeat: return GL_MIRRORED_REPEAT;
	}
	ENGINE_DEBUG_BREAK();
	return GL_NONE;
}

static GLenum get_data_type(enum Data_Type value) {
	switch (value) {
		//
		case Data_Type_s8:  return GL_BYTE;
		case Data_Type_s16: return GL_SHORT;
		case Data_Type_s32: return GL_INT;
		//
		case Data_Type_u8:  return GL_UNSIGNED_BYTE;
		case Data_Type_u16: return GL_UNSIGNED_SHORT;
		case Data_Type_u32: return GL_UNSIGNED_INT;
		//
		case Data_Type_r32: return GL_FLOAT;
		case Data_Type_r64: return GL_DOUBLE;
		//
		case Data_Type_vec2: return GL_FLOAT;
		case Data_Type_vec3: return GL_FLOAT;
		case Data_Type_vec4: return GL_FLOAT;
		//
		case Data_Type_svec2: return GL_INT;
		case Data_Type_svec3: return GL_INT;
		case Data_Type_svec4: return GL_INT;
		//
		case Data_Type_uvec2: return GL_UNSIGNED_INT;
		case Data_Type_uvec3: return GL_UNSIGNED_INT;
		case Data_Type_uvec4: return GL_UNSIGNED_INT;
		//
		case Data_Type_mat2: return GL_FLOAT;
		case Data_Type_mat3: return GL_FLOAT;
		case Data_Type_mat4: return GL_FLOAT;
		//
		case Data_Type_unit_id: return GL_INT;
	}
	ENGINE_DEBUG_BREAK();
	return GL_NONE;
}

static GLenum get_texture_data_type(enum Texture_Type texture_type, enum Data_Type data_type) {
	switch (texture_type) {
		case Texture_Type_Color: switch (data_type) {
			case Data_Type_u8:  return GL_UNSIGNED_BYTE;
			case Data_Type_u16: return GL_UNSIGNED_SHORT;
			case Data_Type_u32: return GL_UNSIGNED_INT;
			case Data_Type_r32: return GL_FLOAT;
			default: return GL_NONE;
		} break;

		case Texture_Type_Depth: switch (data_type) {
			case Data_Type_u16: return GL_UNSIGNED_SHORT;
			case Data_Type_u32: return GL_UNSIGNED_INT;
			case Data_Type_r32: return GL_FLOAT;
			default: return GL_NONE;
		} break;

		case Texture_Type_DStencil: switch (data_type) {
			case Data_Type_u32: return GL_UNSIGNED_INT_24_8;
			case Data_Type_r32: return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
			default: return GL_NONE;
		}

		case Texture_Type_Stencil: switch (data_type) {
			case Data_Type_u8: return GL_UNSIGNED_BYTE;
			default: return GL_NONE;
		}
	}
	ENGINE_DEBUG_BREAK();
	return GL_NONE;
}

static GLenum get_texture_internal_format(enum Texture_Type texture_type, enum Data_Type data_type, u8 channels) {
	switch (texture_type) {
		case Texture_Type_Color: switch (data_type) {
			case Data_Type_u8: switch (channels) {
				case 1: return GL_R8;
				case 2: return GL_RG8;
				case 3: return GL_RGB8;
				case 4: return GL_RGBA8;
				default: return GL_NONE;
			} break;
			case Data_Type_u16: switch (channels) {
				case 1: return GL_R16;
				case 2: return GL_RG16;
				case 3: return GL_RGB16;
				case 4: return GL_RGBA16;
				default: return GL_NONE;
			} break;
			case Data_Type_u32: switch (channels) {
				case 1: return GL_R32UI;
				case 2: return GL_RG32UI;
				case 3: return GL_RGB32UI;
				case 4: return GL_RGBA32UI;
				default: return GL_NONE;
			} break;
			case Data_Type_r32: switch (channels) {
				case 1: return GL_R32F;
				case 2: return GL_RG32F;
				case 3: return GL_RGB32F;
				case 4: return GL_RGBA32F;
				default: return GL_NONE;
			} break;
			default: return GL_NONE;
		} break;

		case Texture_Type_Depth: switch (data_type) {
			case Data_Type_u16: return GL_DEPTH_COMPONENT16;
			case Data_Type_u32: return GL_DEPTH_COMPONENT24;
			case Data_Type_r32: return GL_DEPTH_COMPONENT32F;
			default: return GL_NONE;
		} break;

		case Texture_Type_Stencil: switch (data_type) {
			case Data_Type_u8: return GL_STENCIL_INDEX8;
			default: return GL_NONE;
		}

		case Texture_Type_DStencil: switch (data_type) {
			case Data_Type_u32: return GL_DEPTH24_STENCIL8;
			case Data_Type_r32: return GL_DEPTH32F_STENCIL8;
			default: return GL_NONE;
		}
	}
	ENGINE_DEBUG_BREAK();
	return GL_NONE;
}

static GLenum get_texture_data_format(enum Texture_Type texture_type, u8 channels) {
	switch (texture_type) {
		case Texture_Type_Color: switch (channels) {
			case 1: return GL_RED;
			case 2: return GL_RG;
			case 3: return GL_RGB;
			case 4: return GL_RGBA;
		} break;

		case Texture_Type_Depth:    return GL_DEPTH_COMPONENT;
		case Texture_Type_DStencil: return GL_DEPTH_STENCIL;
		case Texture_Type_Stencil:  return GL_STENCIL_INDEX;
	}
	ENGINE_DEBUG_BREAK();
	return GL_NONE;
}

// static GLenum get_attachment_format(enum Texture_Type texture_type, u8 index) {
// 	switch (texture_type) {
// 		case Texture_Type_Color:    return GL_COLOR_ATTACHMENT0 + index;
// 		case Texture_Type_Depth:    return GL_DEPTH_ATTACHMENT;
// 		case Texture_Type_DStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
// 		case Texture_Type_Stencil:  return GL_STENCIL_ATTACHMENT;
// 	}
// 	ENGINE_DEBUG_BREAK();
// 	return GL_NONE;
// }

static GLenum get_mesh_usage(enum Mesh_Frequency frequency, enum Mesh_Access access) {
	switch (frequency) {
		case Mesh_Frequency_Static: switch (access) {
			case Mesh_Access_Draw: return GL_STATIC_DRAW;
			case Mesh_Access_Read: return GL_STATIC_READ;
			case Mesh_Access_Copy: return GL_STATIC_COPY;
		} break;
		case Mesh_Frequency_Dynamic: switch (access) {
			case Mesh_Access_Draw: return GL_DYNAMIC_DRAW;
			case Mesh_Access_Read: return GL_DYNAMIC_READ;
			case Mesh_Access_Copy: return GL_DYNAMIC_COPY;
		} break;
		case Mesh_Frequency_Stream: switch (access) {
			case Mesh_Access_Draw: return GL_STREAM_DRAW;
			case Mesh_Access_Read: return GL_STREAM_READ;
			case Mesh_Access_Copy: return GL_STREAM_COPY;
		} break;
	}
	ENGINE_DEBUG_BREAK();
	return GL_NONE;
}

// Common
static void impl_Common_Set_Clip_45(u8 const * buffer) {
	GET_VALUE(bool, lower_left)
	GET_VALUE(bool, zero_one)
	glClipControl(
		lower_left ? GL_LOWER_LEFT : GL_UPPER_LEFT,
		zero_one ? GL_ZERO_TO_ONE : GL_NEGATIVE_ONE_TO_ONE
	);
}
static void impl_Common_Set_Clip(u8 const * buffer) {
	if (rvm->version >= OGL_VERSION(4, 5)) {
		impl_Common_Set_Clip_45(buffer); return;
	}
	printf("[wrn] no clip control");
}

static void impl_Common_Set_Viewport(u8 const * buffer) {
	GET_VALUE(svec2, pos)
	GET_VALUE(svec2, size)
	glViewport(pos.x, pos.y, size.x, size.y);
}

// Color
static void impl_Color_Set_Write(u8 const * buffer) {
	GET_VALUE(enum RVM_Color_Write, value)
	glColorMask(
		(value & RVM_Color_Write_R) == RVM_Color_Write_R,
		(value & RVM_Color_Write_G) == RVM_Color_Write_G,
		(value & RVM_Color_Write_B) == RVM_Color_Write_B,
		(value & RVM_Color_Write_A) == RVM_Color_Write_A
	);
}

static void impl_Color_Set_Clear(u8 const * buffer) {
	GET_VALUE(vec4, value)
	glClearColor(value.x, value.y, value.z, value.w);
}

static void impl_Color_Set_Blend(u8 const * buffer) {
	GET_VALUE(enum RVM_Color_Blend, value)
	if (value == RVM_Color_Blend_Opaque) { glDisable(GL_BLEND); return; }
	glEnable(GL_BLEND);
	switch (value) {
		case RVM_Color_Blend_Alpha:      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); break;
		case RVM_Color_Blend_Additive:   glBlendFunc(GL_SRC_ALPHA, GL_ONE);                 break;
		case RVM_Color_Blend_Multiply:   glBlendFunc(GL_DST_COLOR, GL_ZERO);                break;
		case RVM_Color_Blend_PMAlpha:    glBlendFunc(GL_ONE,       GL_ONE_MINUS_SRC_ALPHA); break;
		case RVM_Color_Blend_PMAdditive: glBlendFunc(GL_ONE,       GL_ONE);                 break;
		default: ENGINE_DEBUG_BREAK(); break;
	}
}

// Depth
static void impl_Depth_Set_Read(u8 const * buffer) {
	GET_VALUE(bool, value)
	if (!value) { glDisable(GL_DEPTH_TEST); return; }
	glEnable(GL_DEPTH_TEST);
}

static void impl_Depth_Set_Write(u8 const * buffer) {
	GET_VALUE(bool, value)
	glDepthMask(value);
}

static void impl_Depth_Set_Clear_41(u8 const * buffer) {
	GET_VALUE(r32, value)
	glClearDepthf(value);
}
static void impl_Depth_Set_Clear_20(u8 const * buffer) {
	GET_VALUE(r32, value)
	glClearDepth((double)value);
}
static void impl_Depth_Set_Clear(u8 const * buffer) {
	if (rvm->version >= OGL_VERSION(4, 1)) {
		impl_Depth_Set_Clear_41(buffer); return;
	}
	impl_Depth_Set_Clear_20(buffer);
}

static void impl_Depth_Set_Comparison(u8 const * buffer) {
	GET_VALUE(enum RVM_Comparison, value)
	glDepthFunc(get_comparison(value));
}

static void impl_Depth_Set_Range_41(u8 const * buffer) {
	GET_VALUE(vec2, value)
	glDepthRangef(value.x, value.y);
}
static void impl_Depth_Set_Range_20(u8 const * buffer) {
	GET_VALUE(vec2, value)
	glDepthRange((double)value.x, (double)value.y);
}
static void impl_Depth_Set_Range(u8 const * buffer) {
	if (rvm->version >= OGL_VERSION(4, 1)) {
		impl_Depth_Set_Range_41(buffer); return;
	}
	impl_Depth_Set_Range_20(buffer);
}

// Stencil
static void impl_Stencil_Set_Read(u8 const * buffer) {
	GET_VALUE(bool, value)
	if (!value) { glDisable(GL_STENCIL_TEST); return; }
	glEnable(GL_STENCIL_TEST);
}

static void impl_Stencil_Set_Write(u8 const * buffer) {
	GET_VALUE(u8, value)
	glStencilMask(value);
}

static void impl_Stencil_Set_Clear(u8 const * buffer) {
	GET_VALUE(u8, value)
	glClearStencil(value);
}

static void impl_Stencil_Set_Comparison(u8 const * buffer) {
	GET_VALUE(enum RVM_Comparison, comparison)
	GET_VALUE(u8, reference)
	GET_VALUE(u8, mask)
	glStencilFunc(get_comparison(comparison), reference, mask);
}

static void impl_Stencil_Set_Operation(u8 const * buffer) {
	GET_VALUE(enum RVM_Operation, stencil_fail__depth_any)
	GET_VALUE(enum RVM_Operation, stencil_success__depth_fail)
	GET_VALUE(enum RVM_Operation, stencil_success__depth_success)
	glStencilOp(
		get_operation(stencil_fail__depth_any),
		get_operation(stencil_success__depth_fail),
		get_operation(stencil_success__depth_success)
	);
}

// Vertex
static void impl_Face_Set_Cull(u8 const * buffer) {
	GET_VALUE(enum RVM_Face_Cull, value)
	if (value == RVM_Face_Cull_None) { glDisable(GL_CULL_FACE); return; }
	glEnable(GL_CULL_FACE);
	glCullFace(get_face_cull(value));
}

static void impl_Face_Set_Front(u8 const * buffer) {
	GET_VALUE(enum RVM_Face_Front, value)
	glCullFace(get_face_front(value));
}

// Shader
static void impl_Shader_Allocate(u8 const * buffer) {
	GET_VALUE(struct Ref, ref)

	if (ref.id == REF_EMPTY_ID) { return; }
	if (ref.id >= rvm->shaders_capacity) {
		size_t capacity = ref.id + 1;
		struct VM_Shader * shaders = ENGINE_REALLOC(rvm->shaders, capacity * sizeof(*shaders));

		if (!shaders) { ENGINE_DEBUG_BREAK(); return; }
		memset(shaders + rvm->shaders_capacity, 0, (capacity - rvm->shaders_capacity) * sizeof(*shaders));

		rvm->shaders = shaders;
		rvm->shaders_capacity = capacity;
	}

	struct VM_Shader * shader = rvm->shaders + ref.id;
	if (shader->id) { return; }

	shader->id = glCreateProgram();
}

static void impl_Shader_Free(u8 const * buffer) {
	GET_VALUE(struct Ref, ref)

	if (ref.id == REF_EMPTY_ID) { return; }
	if (ref.id >= rvm->shaders_capacity) { return; }

	struct VM_Shader * shader = rvm->shaders + ref.id;

	if (ref.id == rvm->shader) { glUseProgram(0); rvm->shader = REF_EMPTY_ID; }
	glDeleteProgram(shader->id);

	*shader = (struct VM_Shader){.id = 0};
}

static void impl_Shader_Load(u8 const * buffer) {
	GET_VALUE(struct Ref, ref)

	if (ref.id == REF_EMPTY_ID) { return; }
	if (ref.id >= rvm->shaders_capacity) { return; }

	struct VM_Shader * shader = rvm->shaders + ref.id;
	(void)shader;
}

static void impl_Shader_Use(u8 const * buffer) {
	GET_VALUE(struct Ref, ref)

	if (ref.id == REF_EMPTY_ID) { glUseProgram(0); return; }
	if (ref.id >= rvm->shaders_capacity) { glUseProgram(0); return; }

	struct VM_Shader const * shader = rvm->shaders + ref.id;
	glUseProgram(shader->id);
}

static void impl_Shader_Uniform(u8 const * buffer) {
	GET_VALUE(struct Ref, ref)

	if (ref.id == REF_EMPTY_ID) { return; }
	if (ref.id >= rvm->shaders_capacity) { return; }

	struct VM_Shader * shader = rvm->shaders + ref.id;
	(void)shader;
}

// Mesh
static void impl_Mesh_Allocate(u8 const * buffer) {
	GET_VALUE(struct Ref, ref)
	
	struct Asset_Mesh asset;
	GLenum data_type = get_data_type(asset.type);
	GLenum usage = get_mesh_usage(asset.frequency, asset.access);

	(void)data_type;
	(void)usage;
}

static void impl_Mesh_Free(u8 const * buffer) {
	GET_VALUE(struct Ref, ref)
}

static void impl_Mesh_Load(u8 const * buffer) {
	GET_VALUE(struct Ref, ref)
}

static void impl_Mesh_Use(u8 const * buffer) {
	GET_VALUE(struct Ref, ref)
}

// Target
// static void impl_Target_Allocate(u8 const * buffer) { }
// static void impl_Target_Free(u8 const * buffer) { }
// static void impl_Target_Load(u8 const * buffer) { }
// static void impl_Target_Use(u8 const * buffer) { }
// static void impl_Target_Clear(u8 const * buffer) { }

// Texture
static void impl_Texture_Allocate(u8 const * buffer) {
	GET_VALUE(struct Ref, ref)

	struct Asset_Texture asset;
	GLenum filter_min = get_filter_min(asset.filter_mipmap, asset.filter_min);
	GLenum filter_max = get_filter_max(asset.filter_max);
	GLenum wrap_x = get_wrap_type(asset.wrap_x);
	GLenum wrap_y = get_wrap_type(asset.wrap_y);
	GLenum texture_data_type = get_texture_data_type(asset.kind, asset.type);
	GLenum texture_internal_format = get_texture_internal_format(asset.kind, asset.type, asset.channels);
	GLenum texture_data_format = get_texture_data_format(asset.kind, asset.channels);

	(void)filter_min;
	(void)filter_max;
	(void)wrap_x;
	(void)wrap_y;
	(void)texture_data_type;
	(void)texture_internal_format;
	(void)texture_data_format;
}

static void impl_Texture_Free(u8 const * buffer) {
	GET_VALUE(struct Ref, ref)
}

static void impl_Texture_Load(u8 const * buffer) {
	GET_VALUE(struct Ref, ref)
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
