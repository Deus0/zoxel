// our meta data on realm
entity spawn_realm_dungeon_core(ecs *world, entity wall) {
    // dungeon block, spawns world block prefab first
    // spawn block meta
    entity e = spawn_block_texture(
        world,
        0,
        "dungeon_core",
        "block_dungeon_core");
    zox_set_unique_name(e, "block_dungeon_core");
    // add prefab for world spawning
    entity world_block = spawn_block_world_dungeon(world, wall);
    zox_setv(e, BlockPrefabLink, world_block);
    // its own world prefab (other blocks share global ones), the block owns it, it leaked ending a game
    zox_set_parent(world, world_block, e);
    return e;
}
