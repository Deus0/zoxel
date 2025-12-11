// our meta data on realm
entity spawn_realm_dungeon_core(ecs *world, byte id) {
    // dungeon block, spawns world block prefab first
    // spawn block meta
    entity e = spawn_realm_voxel_texture(
        world,
        id,
        "dungeon_core",
        "block_dungeon_core"
    );
    // add prefab for world spawning
    const entity world_block = spawn_block_world_dungeon(world);
    zox_prefab_set(e, BlockPrefabLink, { world_block });
    // zox_log("spawned dungeon core!")
    return e;
}