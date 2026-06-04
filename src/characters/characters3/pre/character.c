entity spawn_prefab_character3(ecs *world, entity prefab, byte type) {
    if (!prefab) {
        return 0;
    }
    zox_prefab_child(prefab);
    zox_prefab_name("character3");
    zox_add_tag(e, Character);
    zox_add_tag(e, Character3);
    zox_prefab_set(e, Character3Type, { type });
    //  zox_prefab_set(e, CharacterMetaLink, { 0 });
    zox_prefab_set(e, EntityInitialize, { zox_dirty_trigger });
    zox_prefab_set(e, GenerateCharacter, { zox_dirty_trigger });
    // generation
    zox_prefab_set(prefab, Seed, { 999 });
    // name
    zox_prefab_set(e, ZoxName, { 0 });
    // In Terrain
    zox_add_tag(e, LinkChunk);
    zox_prefab_set(e, ChunkLink, { 0 });
    zox_prefab_set(e, ChunkPosition, { int3_chaos });
    zox_prefab_set(e, VoxelPosition, { int3_zero });
    zox_prefab_set(e, Position3DBounds, { float6_zero });
    // Block Inside
    zox_prefab_set(e, InsideBlock, { 0 });
    zox_prefab_set(e, InsideBlockPosition, { int3_zero });
    zox_prefab_set(e, InsideBlockDirty, { 0 });
    zox_prefab_set(e, InsideBlockTime, { 0 });
    // Vox Mesh
    zox_prefab_set(e, BlockScale, { 1 });
    zox_prefab_set(e, ModelLink, { 0 });
    if (type != zox_character_type_instanced) {
        zox_prefab_set(e, ChunkLod, { 255 });
        zox_prefab_set(e, CloneVox, { 0 });
        zox_prefab_set(e, CloneVoxLink, { 0 });
    }
    // animation
    zox_prefab_set(e, AnimationState, { zox_animation_idle });
    zox_prefab_set(e, AnimationStart, { 0 });
    zox_prefab_set(e, ElementLinks, { 0 }); // uis
    // physics3
    zox_add_tag(e, Moveable);
    zox_add_tag(e, Frictioned);
    zox_prefab_set(e, LastPosition3D, { float3_zero });
    zox_prefab_set(e, LastUnstuck3, { float3_zero });
    zox_prefab_set(e, InitializePhysics3D, { 0 });
    zox_prefab_set(e, Velocity3D, { float3_zero });
    zox_prefab_set(e, Acceleration3D, { float3_zero });
    zox_prefab_set(e, Omega3D, { float3_zero });
    zox_prefab_set(e, Alpha3D, { float3_zero });
    zox_prefab_set(e, Gravity3D, { zox_default_gravity });
    zox_prefab_set(e, DisableGravity, { 0 });
    // Bounds
    zox_prefab_set(e, Bounds3D, { float3_single(0) });
    zox_prefab_set(e, Bounds3Dirty, { 0 });
    zox_prefab_set(e, DisableMovement, { 0 });
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
    zox_prefab_set(e, RaycastRange, { 0 }); // raycaster_range
    zox_prefab_set(e, TriggerActionA, { 0 });
    // Debug
    prefab_add_cube_lines(world, e, color_white, 0);
    return e;
}
