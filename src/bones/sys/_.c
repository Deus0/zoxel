#include "settings.c"
#include "generate.c"
#include "paint.c"
#include "bone_render.c"
#include "head_bob.c"
#include "init.c"
#include "arm_control.c"
#include "arm_swing.c"
#include "arm_raise.c"

void define_systems_bones(ecs *world) {
    zox_system(
        HeadAnimateSystem,
        zoxp_update,
        [in] bones.SkeletonDirty,
        [in] bones.HeadBoneLink,
        [none] bones.Skeleton
    );
    zox_system(
        BoneIndexGenerateSystem,
        zoxp_update,
        [in] bones.SkeletonDirty,
        [in] rendering.MeshVertices,
        [in] rendering3.BoneLinks,
        [out] rendering3.BoneIndexes,
        [none] bones.Skeleton
    );
    zox_system(
        BoneIndexGenerateSystem2,
        zoxp_update,
        [in] rendering.MeshVertices,
        [in] rendering3.BoneLinks,
        [out] rendering3.BoneIndexes,
        [none] bones.Skeleton,
        [none] rendering.BuildMeshWeights,
    );
    /*zox_system(
        BonePaintSystem,
        zoxp_update,
        [in] rendering3.BoneIndexes,
        [out] rendering.MeshDirty,
        [out] rendering.MeshColorRGBs,
        [none] bones.Skeleton
    );*/
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
        [out] transforms3.LocalRotation3D,
        [none] bones.ShoulderBone
    );
    zox_system(
        LegSwingSystem,
        zoxp_update,
        [in] bones.SwingState,
        [in] bones.SwingAngle,
        [in] transforms3.LocalPosition3D,
        [out] transforms3.LocalRotation3D,
        [none] bones.ThighBone
    );
    zox_system(
        ShoulderRaiseSystem,
        zoxp_update,
        [in] bones.RaiseShoulder,
        [in] bones.ShoulderBoneLink,
        [in] bones.HeadBoneLink,
        [none] bones.Skeleton
    );
    zox_system_1(
        BonesSettingsSystem,
        zoxp_spawn,
        [in] settings.LoadSettings,
    );
    zox_system(
        BonesSettingsDirtySystem,
        zoxp_update,
        [in] settings.SettingDirty,
        [in] core.ZoxName,
        [in] settings.Setting
    );
    zox_system_1(
        BoneGizmoSystem,
        zoxp_spawn,
        [in] transforms3.Position3D,
        [in] bones.BoneSize,
        [none] bones.Bone
    );
}
