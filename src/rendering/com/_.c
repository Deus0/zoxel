zox_tag(Mesh);
zox_tag(Shader);
zox_tag(Material);
zox_tag(SingleMaterial);
zox_tag(RendererInstance);
zox_tag(VoxMesh);

zoxc_byte(MeshDirty);
zoxc_state(TextureDirty);
zoxc_state(RenderDistanceDirty);
zoxc_state(RenderDepthDirty);
zoxc_state(MeshGenerate);    // TODO: Replace ChunkMeshDirty
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
zoxc_arrayd(MeshIndicies, int)
zoxc_uint(MeshIndiciesGpu);
zoxc_arrayd(MeshVertices, float3)
zoxc_arrayd(MeshVertices2D, float2)
zoxc_arrayd(MeshUVs, float2)
zoxc_arrayd(MeshColors, color)
zoxc_arrayd(MeshColorRGBs, color_rgb)
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

    zoxd_byte(MeshDirty);
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
    zox_define_component_w_dest(ShaderGPULink);
    zox_define_component_w_dest(MaterialGPULink);
    zox_define_component_w_dest(TextureGPULink);
    zox_define_component_w_dest(MeshGPULink);
    zox_define_component_w_dest(UvsGPULink);
    zox_define_component_w_dest(ColorsGPULink);
    zox_define_component_w_dest(ComputeShaderLink);
    zox_define_component_w_dest(UboGPULink);
    zoxd_byte(ShaderSourceIndex);
    zoxd_entity(ShaderLink);
    zoxd_arrayd(MeshIndicies);
    zoxd_uint(MeshIndiciesGpu);
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
