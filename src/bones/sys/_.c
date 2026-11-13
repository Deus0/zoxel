#include "settings.c"
#include "generate.c"
#include "paint.c"
#include "bone_render.c"
#include "head_bob.c"
#include "head_look.c"
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
        skeleton_meshes_system,
        zoxp_spawn,
        [none] bones.Skeleton,
    );
    // Builds skeleton meshes from their parents vox
    zox_system(
        skeleton_mesh_build_system,
        zoxp_octree_read,
        [in] rendering.RenderDepth,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshVertices,
        [out] rendering.MeshColorRGBs,
        [none] rendering3.SkeletonMesh,
        [none] rendering.BuildMesh,
    );
    zox_system(
        head_look_system,
        zoxp_update,
        [in] LookTarget,
        [in] transforms3.Position3D,
        [out] transforms3.LocalRotation3D,
        [none] HeadBone
    );
    zox_system(
        head_animate_system,
        zoxp_update,
        [none] bones.Skeleton,
        [none] bones.SkeletonUpdate,
    );
    zox_system(
        bone_index_generate_system,
        zoxp_update,
        [none] bones.Skeleton,
        [none] bones.SkeletonUpdate,
    );
    zox_system(
        bone_index_generate_system2,
        zoxp_update,
        [in] rendering.MeshVertices,
        [out] rendering3.BoneIndexes,
        [none] rendering3.SkeletonMesh,
        [none] rendering.BuildMeshWeights,
    );
    zox_system(
        arm_control_system,
        zoxp_update,
        [in] bones.WalkState,
        [out] bones.SwingPhase,
        [out] bones.SwingBlend,
        [none] bones.Skeleton
    );
    zox_system(
        arm_swing_system,
        zoxp_update,
        [in] bones.SwingState,
        [in] bones.SwingAngle,
        [in] transforms3.LocalPosition3D,
        [out] transforms3.LocalRotation3D,
        [none] bones.ShoulderBone
    );
    zox_system(
        leg_swing_system,
        zoxp_update,
        [in] bones.SwingState,
        [in] bones.SwingAngle,
        [in] transforms3.LocalPosition3D,
        [out] transforms3.LocalRotation3D,
        [none] bones.ThighBone
    );
    zox_system(
        shoulder_raise_system,
        zoxp_update,
        [in] bones.RaiseShoulder,
        [none] bones.Skeleton
    );
    zox_system_1(
        bone_gizmo_system,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] bones.BoneSize,
        [none] bones.Bone
    );
}
