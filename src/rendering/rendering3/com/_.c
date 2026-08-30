zox_tag(Mesh3);
zox_tag(MeshBasic3D);
zox_tag(TexturedMesh3);
zox_tag(SkeletonMesh);
#include "bone_index.c"
#include "material_bone.c"
zoxc_float3(BonePosition);      // Local to skeleton
zoxc_arrayd(BoneIndexes, byte);
zoxc_arrayd(BoneLinks, entity);

void zox_components_rendering3(ecs* world) {
    zoxd_nf_tag(Mesh3);
    zoxd_nf_tag(MeshBasic3D);
    zoxd_nf_tag(TexturedMesh3);
    zoxd_nf_tag(SkeletonMesh);
    zoxd(MaterialTextured3D);
    zoxd(MaterialColored3D);
    zoxd(MaterialVoxInstance);
    zoxd(MaterialBasic3D);
    zoxd_guint_dest(BoneIndexGPULink);
    zoxd_float3(BonePosition);
    zoxd_arrayd(BoneIndexes);
    zoxd_arrayd(BoneLinks);
    zoxd(MaterialBone);
}
