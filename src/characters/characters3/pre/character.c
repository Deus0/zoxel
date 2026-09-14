entity spawn_prefab_character3(
    ecs *world,
    entity prefab,
    byte type)
{
    entity e = zox_prefab_from_parent(world, prefab);
    zox_prefab_name("character3");
    zox_add(e, Character);
    zox_add(e, Character3);
#ifdef zox_non_fragment_parent
    // zox_add(e, EcsParent);
#endif
    // generation
    zox_setv(e, Seed, 0);
    // name
    zox_setv(e, ZoxName, 0);
    zox_setv(e, GenerateName, 1);
    // In Terrain
    zox_add(e, LinkChunk);
    zox_setv(e, ChunkLink, 0);
    zox_setv(e, ChunkPosition, int3_chaos);
    zox_setv(e, BlockPosition, int3_zero);
    zox_setv(e, Position3DBounds, float6_zero);
    // Block Inside
    zox_setv(e, InsideBlock, 0);
    zox_setv(e, InsideBlockPosition, int3_zero);
    zox_setv(e, InsideBlockDirty, 0);
    zox_setv(e, InsideBlockTime, 0);
    // Vox Mesh
    zox_setv(e, BlockScale, 1);
    zox_setv(e, ModelLink, 0);
    // animation
    zox_setv(e, AnimationState, zox_animation_idle);
    zox_setv(e, AnimationStart, 0);
    zox_setv(e, ElementLinks, 0); // uis
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
    zox_setv(e, RaycastType, 0);
    zox_setv(e, RaycastRange, 1.5f);
    zox_setv(e, TriggerActionA, 0);
    // Debug
    // prefab_add_cube_lines(world, e, color_white, 0);
    // Unique Character
    if (type != zox_character_type_instanced) {
        // zox_setv(e, ChunkLod, 255);
        zox_setv(e, CloneVox, 0);
        zox_setv(e, CloneVoxLink, 0);
    }
    zox_add(e, PreInitialize);
    zox_setv(e, GenerateCharacter, zox_dirty_trigger);
    return e;
}
