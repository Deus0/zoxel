entity dbg_cube_textured;
entity dbg_cube_textured_inspector;

void zox_dbg_spawn_cube_textured(ecs *world) {
    // byte dbg_inspector = 1;
    char* texture_name = "block_dungeon_core";
    float distance = 1;
    float scale = 0.25f;
    if (zox_valid(dbg_cube_textured_inspector)) {
        zox_delete(dbg_cube_textured_inspector);
        dbg_cube_textured_inspector = 0;
    }
    if (dbg_cube_textured) {
        zox_log("+ Deleting [dbg_cube_textured]");
        zox_delete(dbg_cube_textured);
        dbg_cube_textured = 0;
        return;
    }
    entity camera = zox_get_link(world, dbg_player, CameraLink);
    if (!zox_valid(camera)) {
        return;
    }
    zox_log("+ Testing [dbg_cube_textured]");
    entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(texture_name));
    if (!zox_valid(texture)) {
        zox_loge("Failed getting file texture in test [%s]", texture_name);
        return;
    }
    float3 position = zox_getv(camera, Position3D);
    float4 rotation = zox_getv(camera, Rotation3D);
    float3 spawn_position = move_along_direction(position, rotation, -distance);
    entity cube = spawn_cube_textured(
        world,
        prefab_cube_textured,
        texture,
        spawn_position,
        scale);
    add_eternal_euler(world, cube, (float3) { 24, 24, 0 });
    dbg_cube_textured = cube;
    inspect_entity(world, cube);
}
