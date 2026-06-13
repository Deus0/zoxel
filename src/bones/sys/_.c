#include "generate.c"
#include "upload.c"
#include "paint.c"
#include "mesh_render.c"
#include "bone_render.c"
#include "head_bob.c"
#include "init.c"
#include "arm_control.c"
#include "arm_swing.c"
#include "arm_raise.c"

void define_systems_bones(ecs *world) {
    zox_system_1(
        BonesInitializeSystem,
        zoxp_mainthread,
        [in] core.InitializeEntity,
        [out] bones.BoneIndexGPULink,
        [none] bones.Skeleton
    );
    zox_system(
        HeadAnimateSystem,
        EcsOnUpdate,
        [in] bones.SkeletonDirty,
        [in] bones.HeadBoneLink,
        [none] bones.Skeleton
    );
    // generating bone indexes here
    zox_render3D_plus_system(
        Skeleton3RenderSystem,
        [in] rendering.MeshIndicies,
        [in] rendering.MeshGPULink,
        [in] rendering.ColorsGPULink,
        [in] bones.BoneIndexGPULink,
        [in] transforms.TransformMatrix,
        [in] rendering.RenderDisabled,
        [in] bones.BoneLinks,
        [none] rendering3.SkeletonMesh,
        [none] rendering.MeshColorRGBs,
        [none] !rendering.UvsGPULink
    );
    zox_system(
        BoneIndexGenerateSystem,
        EcsOnUpdate,
        [in] bones.SkeletonDirty,
        [in] rendering.MeshDirty,
        [in] rendering.MeshVertices,
        [in] bones.BoneLinks,
        [out] bones.BoneIndexes,
        [none] bones.Skeleton
    );
    zox_system(
        BonePaintSystem,
        EcsPostUpdate,
        [in] rendering.MeshDirty,
        [in] bones.BoneIndexes,
        [out] rendering.MeshColorRGBs,
        [none] bones.Skeleton
    );
    zox_system_1(
        BoneIndexUploadSystem,
        zoxp_mainthread,
        [in] rendering.MeshDirty,
        [in] bones.BoneIndexes,
        [out] bones.BoneIndexGPULink
    );
    zox_system_1(
        BoneRenderSystem,
        zoxp_mainthread,
        [in] transforms3.Position3D,
        [in] bones.BoneSize,
        [none] bones.Bone
    );
    zox_system(
        ArmControlSystem,
        EcsOnUpdate,
        [in] bones.WalkState,
        [none] bones.Skeleton
    );
    zox_system(
        ArmSwingSystem,
        EcsOnUpdate,
        [in] bones.SwingState,
        [in] bones.SwingAngle,
        [in] transforms3.LocalPosition3D,
        [out] transforms3.LocalRotation3D,
        [none] bones.ShoulderBone
    );
    zox_system(
        LegSwingSystem,
        EcsOnUpdate,
        [in] bones.SwingState,
        [in] bones.SwingAngle,
        [in] transforms3.LocalPosition3D,
        [out] transforms3.LocalRotation3D,
        [none] bones.ThighBone
    );
    zox_system(
        ShoulderRaiseSystem,
        EcsPostUpdate,
        [in] bones.RaiseShoulder,
        [in] bones.ShoulderBoneLink,
        [in] bones.HeadBoneLink,
        [none] bones.Skeleton
    );
}
