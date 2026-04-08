entity dbg_cube = 0;

extern void add_eternal_euler(ecs*, entity, float3);

void keydown_test_cubes(ecs *world, int32_t keycode) {

    char* tname = "block_dungeon_core";
    float3 spawn_position = float3_zero;

    if (keycode == SDLK_1) {

        if (dbg_cube) {
            zox_log("> Cleaning [spawn_cube]");
            zox_delete(dbg_cube);
            dbg_cube = 0;
        } else {
            zox_log("> Testing [spawn_cube]");
            entity e = spawn_cube(world, prefab_cube, spawn_position, 1);
            add_eternal_euler(world, e, (float3) { 0, 25, 0 });
            dbg_cube = e;
        }
    }

    if (keycode == SDLK_2) {

        if (dbg_cube) {
            zox_log("> Cleaning [spawn_cube_textured]");
            zox_delete(dbg_cube);
            dbg_cube = 0;
        } else {
            zox_log("> Testing [spawn_cube_textured]");
            entity texture = string_hashmap_get(files_hashmap_textures, new_string_data(tname));

            entity e = spawn_cube_textured(world, prefab_cube_textured, spawn_position, texture, 1);
            add_eternal_euler(world, e, (float3) { 0, 25, 0 });
            dbg_cube = e;

            zox_log("- Texture [%s] ? [%s]", tname, texture ? zox_get_name(texture) : "Not Found");
        }
    }
}
