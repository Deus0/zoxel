// NOTE: Characters hold no voxel or mesh data, those are children:
//  - with a model: a renderer child (RendererLink), see CharacterChildrenSpawnSystem
//  - CharacterUnique: also a cloned vox child (VoxLink) with its own lod meshes
//  - SkeletonOwner: a combined vox child (VoxLink) and a skeleton child (SkeletonLink), from bones
entity spawn_prefab_character3(ecs *world) {
    zox_prefab();
    zox_prefab_name("character3");
    zox_add(e, Character);
    zox_add(e, Character3);
    // Transforms
    zox_setv(e, Position3D, float3_zero);
    zox_setv(e, Rotation3D, float4_identity);
    zox_setv(e, Scale1, 1);
    zox_setv(e, TransformMatrix, float4x4_identity);
    // Render State, children follow these
    zox_setv(e, RenderDisabled, 0);
    zox_setv(e, RenderDepth, 0);
    zox_setv(e, MaxRenderDepth, 0);
    zox_setv(e, RenderDepthDirty, zox_dirty_none);
    // generation
    zox_setv(e, Seed, 0);
    // name
    zox_setv(e, ZoxName, 0);
    // zox_setv(e, GenerateName, 1);
    // In Terrain
    zox_add(e, LinkChunk);
    zox_setv(e, ChunkPosition, int3_chaos);
    zox_setv(e, BlockPosition, int3_zero);
    zox_setv(e, Position3DBounds, float6_zero);
    // Block Inside
    zox_setv(e, InsideBlock, 0);
    zox_setv(e, InsideBlockPosition, int3_zero);
    zox_setv(e, InsideBlockTime, 0);
    // Vox Mesh
    zox_setv(e, BlockScale, 1);
    // animation
    zox_setv(e, AnimationState, zox_animation_idle);
    zox_setv(e, AnimationStart, 0);
    // physics3
    zox_add(e, Moveable);
    zox_add(e, Frictioned);
    zox_setv(e, LastPosition3D, float3_zero);
    zox_setv(e, LastUnstuck3, float3_zero);
    zox_setv(e, InitializePhysics3D, 0);
    zox_setv(e, Velocity3D, float3_zero);
    zox_setv(e, Acceleration3D, float3_zero);
    zox_setv(e, Omega3D, float3_zero);
    zox_setv(e, Alpha3D, float3_zero);
    zox_setv(e, Gravity3D, zox_default_gravity);
    // Bounds
    zox_setv(e, Bounds3D, float3_single(0.5f));
    zox_setv(e, Bounds3Dirty, 0);
    // zox_setv(e, DisableMovement, 0);
    zox_setv(e, Grounded, 0);
    zox_setv(e, Collision, 0);
    zox_setv(e, CollisionDistance, float3_zero);
    // jump
    zox_setv(e, CanJump, 0);
    zox_setv(e, JumpState, 0);
    zox_setv(e, LandState, 0);
    zox_setv(e, Jump, 0);
    // Triggers
    zox_setv(e, RaycastVoxelData, 0);
    zox_setv(e, RaycastType, zox_raycast_type_select);
    zox_setv(e, RaycastRange, 1.5f);
    zox_setv(e, TriggerActionA, 0);
    // Debug
    // prefab_add_cube_lines(world, e, color_white, 0);
    // Children
    zox_add(e, VoxOwner);
    zox_add(e, PreInitialize);
    zox_setv(e, GenerateCharacter, zox_dirty_trigger);
    return e;
}
