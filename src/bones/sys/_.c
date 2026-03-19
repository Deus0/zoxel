#include "skeleton_render3D_system.c"
#include "bone_index_generate_system.c"
#include "bone_index_upload_system.c"
#include "bone_paint_system.c"
#include "render_bones.c"

void define_systems_bones(ecs *world) {
    // generating bone indexes here
    if (headless) {
        return;
    }

    zox_render3D_plus_system(
        Skeleton3RenderSystem,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshGPULink,
        [in] rendering.ColorsGPULink,
        [in] BoneIndexGPULink,
        [in] transforms3.TransformMatrix,
        [in] rendering.RenderDisabled,
        [in] BoneLinks,
        [none] rendering3.SkeletonMesh,
        [none] rendering.MeshColorRGBs,
        [none] !rendering.UvsGPULink
    );
    zox_system(
        BoneIndexGenerateSystem,
        EcsOnUpdate,
        [in] rendering.MeshDirty,
        [in] rendering.MeshVertices,
        [in] BoneLinks,
        [out] BoneIndexes
    );
    zox_system(
        BonePaintSystem,
        EcsPostUpdate,
        [in] rendering.MeshDirty,
        [in] BoneIndexes,
        [out] rendering.MeshColorRGBs,
        [none] Skeleton
        // [none] PaintedSkeleton
    );
    zox_system_1(
        BoneIndexUploadSystem,
        zoxp_mainthread,
        [in] rendering.MeshDirty,
        [in] BoneIndexes,
        [out] BoneIndexGPULink
    );
    zox_system_1(
        BoneRenderSystem,
        zoxp_mainthread,
        // [in] bones.SkeletonLink,
        [in] hierarchys.ParentLink,
        [in] transforms3.Position3D,
        [none] bones.Bone
    );
}
