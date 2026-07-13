zox_tag(Mesh);
zox_tag(Shader);
zox_tag(Material);
zox_tag(SingleMaterial);
zox_tag(RendererInstance);
zox_tag(VoxMesh);
zox_tag(DisableDepthTest);
zoxc_byte(RenderOrder);
zoxc_byte(MeshDirty);
zoxc_byte(MeshReady);
zoxc_state(TextureDirty);
zoxc_state(RenderDistanceDirty);
zoxc_state(RenderDepthDirty);
zoxc_state(MeshGenerate);    // TODO: Replace BuildChunkMesh
zoxc_state(MeshColorsGenerate);
zoxc_state(MeshColorsDirty);
zoxc_entity(MeshLink);
zoxc_byte(MeshAlignment);
zoxc_byte(RenderDepth);
zoxc_byte(MaxRenderDepth);
zoxc_byte(RenderDistance);
zoxc_byte(RenderDisabled);
zoxc_float(Brightness);
zoxc_float(Alpha);
zoxc_entity(MaterialLink);
zoxc_int2(TextureSize);
zoxc_byte(ShaderSourceIndex);
zoxc_entity(ShaderLink); // links to a shader entity
zoxc_entity(InstanceLink); // links to a instance parent
// zoxc_array(InstanceLinks, entity, 8)
zoxc_arrayd(MeshIndicies, int);
zoxc_uint(MesnRenderCount);
zoxc_arrayd(MeshVertices, float3);
zoxc_arrayd(MeshVertices2D, float2);
zoxc_arrayd(MeshUVs, float2);
zoxc_arrayd(MeshColors, color);
zoxc_arrayd(MeshColorRGBs, color_rgb);
#include "material_gpu_link.c"
#include "texture_gpu_link.c"
#include "ubo_gpu_link.c"
#include "mesh_gpu_link.c"
#include "uvs_gpu_link.c"
#include "colors_gpu_link.c"
#include "shader_gpu_link.c"
#include "compute_shader.c"
// Models
zoxc_entities(ModelLinks)
zoxc_entity(ModelLink);                 // a render instance links to model
#define model_lods_max_length 8
zoxc_array(ModelLods, entity, model_lods_max_length);       // a model has multiple models per LOD

static inline int2 get_texture_size(ecs* world, entity e) {
    return zox_get_value(e, TextureSize);
}

void define_components_rendering(ecs *world) {
    zoxd_tag(Mesh);
    zoxd_tag(Shader);
    zoxd_tag(Material);
    zoxd_tag(SingleMaterial);
    zoxd_tag(RendererInstance);
    zoxd_tag(VoxMesh);
    zoxd_tag(DisableDepthTest);
    zoxd_byte(RenderOrder);
    zoxd_byte(MeshDirty);
    zoxd_byte(MeshReady);
    zoxd_state(TextureDirty);
    zoxd_state(RenderDistanceDirty);
    zoxd_state(RenderDepthDirty);
    zoxd_state(MeshGenerate);
    zoxd_state(MeshColorsGenerate);
    zoxd_state(MeshColorsDirty);
    zoxd_byte(MeshAlignment);
    zoxd_entity(MeshLink);
    zoxd_byte(RenderDepth);
    zoxd_byte(MaxRenderDepth);
    zoxd_byte(RenderDistance);
    zoxd_byte(RenderDisabled);
    zoxd_float(Brightness);
    zoxd_float(Alpha);
    zoxd_entity(MaterialLink);
    zoxd_int2(TextureSize);
    zoxd_entity(InstanceLink);
    zoxd(MaterialInstancedGPULink);
    zoxd_uint_dest(ShaderGPULink);
    zoxd_uint_dest(MaterialGPULink);
    zoxd_uint_dest(TextureGPULink);
    zoxd_uint_dest(UvsGPULink);
    zoxd_uint_dest(ColorsGPULink);
    zoxd_uint_dest(ComputeShaderLink);
    zoxd_uint_dest(UboGPULink);
    zoxd_uint2_dest(MeshGPULink);
    zoxd_byte(ShaderSourceIndex);
    zoxd_entity(ShaderLink);
    zoxd_arrayd(MeshIndicies);
    zoxd_uint(MesnRenderCount);
    zoxd_arrayd(MeshVertices);
    zoxd_arrayd(MeshVertices2D);
    zoxd_arrayd(MeshUVs);
    zoxd_arrayd(MeshColors);
    zoxd_arrayd(MeshColorRGBs);
    // Models
    zoxd_entities(ModelLinks);
    zoxd_entity(ModelLink);
    zox_define_component_array(ModelLods);
}
