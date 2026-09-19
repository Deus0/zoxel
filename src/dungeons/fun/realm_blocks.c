void spawn_dungeon_blocks(
    ecs *world,
    entity e)
{
    entity wall;
    {
        entity e2 = spawn_block_texture(
            world,
            0,
            "dark",
            "block_dark");
        zox_set_unique_name(e2, "block_dark_bricks");
        zox_add(e2, BlockDungeonBricks);
        zox_set_parent(world, e2, e);
        wall = e2;
    }
    {
        entity e2 = spawn_realm_dungeon_core(world, wall);
        zox_set_parent(world, e2, e);
        zox_add(e2, BlockDungeonCore);
    }
}
