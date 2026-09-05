entity dbg_cube;
entity dbg_cube_inspector;

void zox_dbg_spawn_cube(ecs *world) {
    byte dbg_inspector = 1;
    float distance = 1;
    float scale = 0.25f;
    if (zox_valid(dbg_cube_inspector)) {
        zox_delete(dbg_cube_inspector);
        dbg_cube_inspector = 0;
    }
    if (dbg_cube) {
        zox_log("+ Deleting [dbg_cube]");
        zox_delete(dbg_cube);
        dbg_cube = 0;
        return;
    }
    entity camera = zox_get_link(world, dbg_player, Camera);
    if (!zox_valid(camera)) {
        return;
    }
    zox_log("+ Testing [dbg_cube]");
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(position, rotation, -distance);
    entity cube = spawn_cube(
        world,
        prefab_cube,
        spawn_position,
        scale);
    add_eternal_euler(world, cube, (float3) { 24, 24, 0 });
    if (dbg_inspector) {
        entity player = dbg_player;
        entity canvas = zox_get_link(world, player, Canvas);
        dbg_cube_inspector = spawn_inspector(world, canvas, player, cube);
    }
    dbg_cube = cube;
}
