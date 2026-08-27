entity spawn_prefab_character3(ecs *world, entity prefab, byte type) {
    zox_prefab_child(prefab);
    zox_prefab_name("character3");
    zox_add(e, Character);
    zox_add(e, Character3);
    // generation
    zox_prefab_set(e, Seed, { 0 });
    // name
    zox_prefab_set(e, ZoxName, { 0 });
    zox_prefab_set(e, GenerateName, { 1 });
    // In Terrain
    zox_add(e, LinkChunk);
    zox_prefab_set(e, ChunkLink, { 0 });
    zox_prefab_set(e, ChunkPosition, { int3_chaos });
    zox_prefab_set(e, BlockPosition, { int3_zero });
    zox_prefab_set(e, Position3DBounds, { float6_zero });
    // Block Inside
    zox_prefab_set(e, InsideBlock, { 0 });
    zox_prefab_set(e, InsideBlockPosition, { int3_zero });
    zox_prefab_set(e, InsideBlockDirty, { 0 });
    zox_prefab_set(e, InsideBlockTime, { 0 });
    // Vox Mesh
    zox_prefab_set(e, BlockScale, { 1 });
    zox_prefab_set(e, ModelLink, { 0 });
    // animation
    zox_prefab_set(e, AnimationState, { zox_animation_idle });
    zox_prefab_set(e, AnimationStart, { 0 });
    zox_prefab_set(e, ElementLinks, { 0 }); // uis
    // physics3
    zox_add(e, Moveable);
    zox_add(e, Frictioned);
    zox_prefab_set(e, LastPosition3D, { float3_zero });
    zox_prefab_set(e, LastUnstuck3, { float3_zero });
    zox_prefab_set(e, InitializePhysics3D, { 0 });
    zox_prefab_set(e, Velocity3D, { float3_zero });
    zox_prefab_set(e, Acceleration3D, { float3_zero });
    zox_prefab_set(e, Omega3D, { float3_zero });
    zox_prefab_set(e, Alpha3D, { float3_zero });
    zox_prefab_set(e, Gravity3D, { zox_default_gravity });
    // Bounds
    zox_prefab_set(e, Bounds3D, { float3_single(0.5f) });
    zox_prefab_set(e, Bounds3Dirty, { 0 });
    // zox_prefab_set(e, DisableMovement, { 0 });
    zox_prefab_set(e, Grounded, { 0 });
    zox_prefab_set(e, Collision, { 0 });
    zox_prefab_set(e, CollisionDistance, { float3_zero });
    // jump
    zox_prefab_set(e, CanJump, { 0 });
    zox_prefab_set(e, JumpState, { 0 });
    zox_prefab_set(e, LandState, { 0 });
    zox_prefab_set(e, Jump, { 0 });
    // Triggers
    zox_prefab_set(e, RaycastVoxelData, { 0 });
    zox_prefab_set(e, RaycastType, { 0 });
    zox_prefab_set(e, RaycastRange, { 1.5f });
    zox_prefab_set(e, TriggerActionA, { 0 });
    // Debug
    // prefab_add_cube_lines(world, e, color_white, 0);
    // Unique Character
    if (type != zox_character_type_instanced) {
        zox_prefab_set(e, ChunkLod, { 255 });
        zox_prefab_set(e, CloneVox, { 0 });
        zox_prefab_set(e, CloneVoxLink, { 0 });
    }
    zox_add(e, PreInitialize);
    zox_prefab_set(e, GenerateCharacter, { zox_dirty_trigger });
    return e;
}
