zox_tag(Mesh);
zox_tag(Shader);
zox_tag(Material);
zox_tag(SingleMaterial);
zox_tag(RendererInstance);
zox_tag(VoxMesh);

zoxc_byte(MeshDirty);
zoxc_byte(MeshGenerate);    // TODO: Replace ChunkMeshDirty
zoxc_byte(MeshColorsGenerate);
zoxc_byte(MeshColorsDirty);

zoxc_entity(MeshLink);
zoxc_byte(TextureDirty);
zoxc_byte(MeshAlignment);
zoxc_byte(RenderDepth);
zoxc_byte(RenderDepthDirty);
zoxc_byte(MaxRenderDepth);
zoxc_byte(RenderDistance);
zoxc_byte(RenderDisabled);
zoxc_byte(RenderDistanceDirty);
zoxc_float(Brightness);
zoxc_float(Alpha);
zoxc_entity(MaterialLink);
zoxc_int2(TextureSize);
zoxc_byte(ShaderSourceIndex);
zoxc_entity(ShaderLink); // links to a shader entity
zoxc_entity(InstanceLink); // links to a instance parent
// zoxc_array(InstanceLinks, entity, 8)
zoxc_arrayd(MeshIndicies, int)
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
zoxc_array(ModelLods, entity, 6);       // a model has multiple models per LOD

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
    zoxd_byte(MeshGenerate);
    zoxd_byte(MeshColorsGenerate);
    zoxd_byte(MeshColorsDirty);
    zoxd_byte(MeshAlignment);

    zoxd_entity(MeshLink);
    zoxd_byte(TextureDirty);
    zoxd_byte(RenderDepth);
    zoxd_byte(RenderDepthDirty);
    zoxd_byte(MaxRenderDepth);
    zoxd_byte(RenderDistance);
    zoxd_byte(RenderDistanceDirty);
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
    zoxd_arrayd(MeshVertices);
    zoxd_arrayd(MeshVertices2D);
    zoxd_arrayd(MeshUVs);
    zoxd_arrayd(MeshColors);
    zoxd_arrayd(MeshColorRGBs);
    // Models
    zox_define_entities_component(ModelLinks);
    zoxd_entity(ModelLink);
    zox_define_component_array(ModelLods);
}