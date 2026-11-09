zox_tag(Mesh3);
zox_tag(MeshBasic3);
zox_tag(TexturedMesh3);
zox_tag(SkeletonMesh);
zox_tag(MeshCloneLink);     // links a mesh clone to its source mesh
zox_tag(MeshCloned);        // source mesh has clones linked to it
zox_tag(SwayMesh);          // instance mesh renders with the sway shader (grass)
zoxc_float2(SwayBounds);    // mesh bottom y, mesh height - for sway top verts
#include "bone_index.c"
#include "material_bone.c"
zoxc_float3(BonePosition);      // Local to skeleton
zoxc_arrayd(BoneIndexes, byte);
zoxc_arrayd(BoneLinks, entity);

void zox_components_rendering3(ecs* world) {
    // Types
    zoxd_tag(Mesh3);
    zoxd_tag(MeshBasic3);
    zoxd_tag(TexturedMesh3);
    zoxd_tag(SkeletonMesh);
    // Links
    zoxd_nf_tag(MeshCloneLink);
    zoxd_tag(MeshCloned);
    zoxd_tag(SwayMesh);
    zoxd_float2(SwayBounds);
    zoxd(MaterialTextured3D);
    zoxd(MaterialColored3D);
    zoxd(MaterialVoxInstance);
    zoxd(MaterialVoxInstanceSway);
    zoxd(MaterialBasic3D);
    zoxd_guint_dest(BoneIndexGPULink);
    zoxd_float3(BonePosition);
    zoxd_arrayd(BoneIndexes);
    zoxd_arrayd(BoneLinks);
    zoxd(MaterialBone);
}
