#include "settings.c"
#include "generate.c"
#include "paint.c"
#include "bone_render.c"
#include "head_bob.c"
#include "init.c"
#include "arm_control.c"
#include "arm_swing.c"
#include "arm_raise.c"
#include "skeleton_meshes.c"
zoxc_system_dirty(Skeleton);

void zox_systems_bones(ecs *world) {
    zoxd_system_dirty(Skeleton);
    // Spawns, reuses and swaps a skeleton mesh per render depth
    zox_system_1(
        SkeletonMeshesSystem,
        zoxp_spawn,
        [none] bones.Skeleton,
    );
    // Builds skeleton meshes from their parents vox
    zox_system(
        SkeletonMeshBuildSystem,
        zoxp_octree_read,
        [in] rendering.RenderDepth,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshColorRGBs,
        [none] rendering3.SkeletonMesh,
        [none] rendering.BuildMesh,
    );
    zox_system(
        HeadAnimateSystem,
        zoxp_update,
        [none] bones.Skeleton,
        [none] bones.SkeletonUpdate,
    );
    zox_system(
        BoneIndexGenerateSystem,
        zoxp_update,
        [none] bones.Skeleton,
        [none] bones.SkeletonUpdate,
    );
    zox_system(
        BoneIndexGenerateSystem2,
        zoxp_update,
        [in] rendering.MeshVertices,
        [out] rendering3.BoneIndexes,
        [none] rendering3.SkeletonMesh,
        [none] rendering.BuildMeshWeights,
    );
    zox_system(
        ArmControlSystem,
        zoxp_update,
        [in] bones.WalkState,
        [none] bones.Skeleton
    );
    zox_system(
        ArmSwingSystem,
        zoxp_update,
        [in] bones.SwingState,
        [in] bones.SwingAngle,
        [in] transforms3.LocalPosition3D,
        [out] bones.SwingPhase,
        [out] bones.SwingBlend,
        [out] transforms3.LocalRotation3D,
        [none] bones.ShoulderBone
    );
    zox_system(
        LegSwingSystem,
        zoxp_update,
        [in] bones.SwingState,
        [in] bones.SwingAngle,
        [in] transforms3.LocalPosition3D,
        [out] bones.SwingPhase,
        [out] bones.SwingBlend,
        [out] transforms3.LocalRotation3D,
        [none] bones.ThighBone
    );
    zox_system(
        ShoulderRaiseSystem,
        zoxp_update,
        [in] bones.RaiseShoulder,
        [none] bones.Skeleton
    );
    zox_system_1(
        BoneGizmoSystem,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] bones.BoneSize,
        [none] bones.Bone
    );
}
