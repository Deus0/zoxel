void spawn_dungeon_blocks(ecs *world, entity e, BlockLinks* blocks) {
    entity wall;
    {
        entity e2 = spawn_block_texture(world, 0, "dark", "block_dark");
        zox_set_unique_name(e2, "block_dark_bricks");
        zox_add_tag(e2, BlockDungeonBricks);
        add_to_BlockLinks(blocks, e2);
        wall = e2;
    }
    {
        entity e2 = spawn_realm_dungeon_core(world, wall);
        zox_add_tag(e2, BlockDungeonCore);
        add_to_BlockLinks(blocks, e2);
    }
}
