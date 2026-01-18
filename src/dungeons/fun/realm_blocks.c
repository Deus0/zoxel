void spawn_block_dungeon_blocks(ecs *world, spawn_blocks_data *data) {
    // zox_log("spawn_block_dungeon_core")
    zox_block_dark = data->blocks->length + 1;

    entity dungeon_brick = spawn_block_texture(
        world,
        zox_block_dark,
        "dark",
        "block_dark"
    );
    add_to_BlockLinks(data->blocks, dungeon_brick);

    zox_block_dungeon_core = data->blocks->length + 1;
    entity dungeon_core = spawn_realm_dungeon_core(world, zox_block_dungeon_core);
    add_to_BlockLinks(data->blocks, dungeon_core);
}