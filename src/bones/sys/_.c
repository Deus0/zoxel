#include "settings.c"
#include "generate.c"
#include "bone_render.c"
#include "head_bob.c"
#include "head_look.c"
#include "swing_mark.c"
#include "init.c"
#include "arm_swing.c"
#include "arm_control.c"
#include "arm_raise.c"
#include "skeleton_meshes.c"
#include "skeleton_gpu.c"
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
        [out] rendering.MeshColors,
        [none] rendering3.SkeletonMesh,
        [none] rendering.BuildMesh,
        [none] !rendering.GpuMesh,
    );
    // meshes and weights gpu skeleton meshes on the gpu (sys/skeleton_gpu.c)
    zox_system_1(
        skeleton_mesh_gpu_system,
        zoxp_gpu_upload,
        [in] rendering.RenderDepth,
        [in] rendering.MeshGPULink,
        [in] rendering.ColorsGPULink,
        [in] rendering3.BoneIndexGPULink,
        [out] rendering.MeshFaceRoom,
        [out] rendering.MeshRenderCount,
        [out] rendering.MeshVertices,
        [none] rendering3.SkeletonMesh,
        [none] rendering.GpuMesh,
        [none] rendering.BuildMesh,
        [none] !core.PreInitialize,
        [none] !core.Initialize,
    );
    // a swing passing its marks sets SwingMark (main thread: observers react, spawning sounds and particles); in the
    //  spawn phase with the other main thread ones (in update its sync point split the threaded ones, tst bone_rebuild)
    zox_system_1(
        swing_mark_system,
        zoxp_spawn,
        [in] SwingStart,
        [in] SwingSpeed,
        [in] SwingKind,
        [in] SwingMark,
        [none] Skeleton
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
        [out] rendering.MeshVertices,
        [out] rendering3.BoneIndexes,
        [out] rendering.MeshIndicies,
        [out] rendering.MeshColors,
        [none] rendering3.SkeletonMesh,
        [none] rendering.BuildMeshWeights,
        [none] !rendering.GpuMesh,
    );
    zox_system(
        arm_control_system,
        zoxp_update,
        [in] bones.WalkState,
        [out] bones.SwingPhase,
        [out] bones.SwingBlend,
        [in] bones.CrouchState,
        [out] bones.CrouchBlend,
        [none] bones.Skeleton
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
