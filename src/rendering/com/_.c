// Types
zox_tag(Mesh);
// a render system that can be split across threads (vulkan records each part apart): its draws need no order
//  between its entities and it keeps no state across them (i.e. opaque chunks, not sorted see through ones)
zox_tag(RenderParallel);
zox_tag(Shader);
zox_tag(Material);
zox_tag(SingleMaterial);
zox_tag(RendererInstance);
zox_tag(VoxMesh);
zox_tag(ChunkMesh);
zox_tag(TransparentMesh);   // drawn blended, after the solid ones (a water chunk mesh)
zox_tag(MeshTextureLink);   // a mesh's own texture, bound instead of its material's (a liquid's chunk mesh)
zox_tag(RenderAtCamera);    // drawn round each camera that renders it, its position the camera's (the skybox)
// Events
zox_tag(DisableDepthTest);
zox_tag(MeshClearCache);
zox_tag(BuildMeshColors);
zox_tag(MeshColorsDirty);
zox_tag(RenderTextureDirty);
// Properties
zox_tag(MeshBuilt);
zox_tag(MeshColorsBuilt);
// Links
zox_tag(PreparingMesh);
zox_tag(ActiveMesh);
// Properties
zoxc_int2(TextureSize);
zoxc_float(Brightness);
zoxc_float(Alpha);
zoxc_byte(RenderOrder);
zoxc_byte(MeshAlignment);
zoxc_byte(RenderDepth);
zoxc_byte(MaxRenderDepth);
zoxc_byte(RenderDistance);
zoxc_byte(RenderDisabled);
// Links
zox_tag(MaterialLink);
zoxc_byte(ShaderSourceIndex);
zox_tag(ShaderLink); // links to a shader entity
zox_tag(InstanceLink);     // links an instance renderer to the lod mesh it draws
zox_tag(RendererLink);     // links to a child renderer, i.e. a characters instance renderer
// Models
zox_tag(ModelLink);          // links to a model, or a model group
#define model_lods_max_length 8
zoxc_array(ModelLods, entity, model_lods_max_length);
// Render Data
zoxc_uint(MeshRenderCount);
zoxc_arrayd(MeshIndicies, int);
zoxc_arrayd(MeshVertices, float3);
zoxc_arrayd(MeshVertices2D, float2);
zoxc_arrayd(MeshUVs, float2);
zoxc_arrayd(MeshColors, color);
zoxc_arrayd(MeshColorRGBs, color_rgb);
// GPU Links
#include "material_gpu_link.c"
#include "texture_gpu_link.c"
#include "ubo_gpu_link.c"
#include "mesh_gpu_link.c"
#include "uvs_gpu_link.c"
#include "colors_gpu_link.c"
#include "shader_gpu_link.c"
#include "gpu_mesh.c"
// Events
zox_tag(BuildMesh);
zox_tag(BuildMeshWeights);
zox_tag(MeshDirty);
zox_tag(SkeletonMeshDirty);
// zoxc_state_remove
zoxc_state(RenderDepthDirty);
zoxc_state(RenderDistanceDirty);

static inline int2 get_texture_size(ecs* world, entity e) {
    return zox_getv(e, TextureSize);
}

void zox_components_rendering(ecs *world) {
    zoxd_tag(Mesh);
    zoxd_tag(RenderParallel);
    zoxd_tag(Shader);
    zoxd_tag(Material);
    zoxd_tag(SingleMaterial);
    zoxd_tag(RendererInstance);
    zoxd_tag(VoxMesh);
    zoxd_tag(ChunkMesh);
    zoxd_tag(TransparentMesh);
    zoxd_tag(RenderAtCamera);
    zoxd_nf_link(MeshTextureLink);
    // Properties
    zoxd_tag(DisableDepthTest);
    zoxd_tag(MeshClearCache);
    // Links
    zoxd_nf_link(PreparingMesh);
    zoxd_nf_link(ActiveMesh);
    // zoxd_nf_tag
    // Events
    zoxd_tag_event(BuildMesh);
    zoxd_tag_event(BuildMeshColors);
    zoxd_tag_event(BuildMeshWeights);
    zoxd_tag_event(MeshColorsDirty);
    zoxd_tag_event(RenderTextureDirty);
    zoxd_tag_event(MeshDirty);
    zoxd_tag_event(SkeletonMeshDirty);
    zoxd_state(RenderDistanceDirty);
    zoxd_state(RenderDepthDirty);
    // Properties (Not used in system queries)
    zoxd_nf_tag(MeshBuilt);
    zoxd_nf_tag(MeshColorsBuilt);
    // zox_dont_fragment(RenderDepthDirty);
    // data
    zoxd_byte(RenderOrder);
    // Properties
    zoxd_int2(TextureSize);
    zoxd_byte(MeshAlignment);
    zoxd_byte(RenderDepth);
    zoxd_byte(MaxRenderDepth);
    zoxd_byte(RenderDistance);
    zoxd_byte(RenderDisabled);
    zoxd_float(Brightness);
    zoxd_float(Alpha);
    // Links
    zoxd_byte(ShaderSourceIndex);
    zoxd_nf_link(MaterialLink);
    zoxd_nf_link(InstanceLink);
    zoxd_nf_link(RendererLink);
    zoxd_nf_link(ShaderLink);
    zoxd_nf_link(ModelLink);
    zox_define_component_array(ModelLods);
    // Render Data
    zoxd_uint(MeshRenderCount);
    zoxd_arrayd(MeshIndicies);
    zoxd_arrayd(MeshVertices);
    zoxd_arrayd(MeshVertices2D);
    zoxd_arrayd(MeshUVs);
    zoxd_arrayd(MeshColors);
    zoxd_arrayd(MeshColorRGBs);
    // GPU Links
    zoxd_guint_dest(MaterialGPULink);
    zoxd_guint_dest(TextureGPULink);
    zoxd_guint2_dest(MeshGPULink);
    zoxd_guint_dest(UvsGPULink);
    zoxd_guint_dest(ColorsGPULink);
    zoxd_guint_dest(UboGPULink);
    zoxd_guint_dest(ShaderGPULink);
    // gpu meshes
    zoxd_tag(GpuMesh);
    zoxd_guint_dest(MeshCommandGPULink);
    zoxd_uint(MeshFaceRoom);
    zoxd_dest(MeshFence);
}
