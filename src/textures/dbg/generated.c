/*entity dbg_texture_generated = 0;

extern entity prefab_element_frame;

void spawn_dbg_texture_generated(ecs *world, int32_t keycode) {

    if (keycode != zox_key_0) {
        return;
    }

    if (dbg_texture_generated) {

        zox_log("Deleting [dbg_texture_generated]");
        zox_delete(dbg_texture_generated);
        dbg_texture_generated = 0;

    } else {

        entity player = dbg_player;
        if (!zox_valid(player)) {
            return;
        }

        entity canvas = get_linked_canvas(world, player);

        int2 position = int2_single(0);
        int2 size = int2_single(512);
        int2 tsize = int2_single(64);
        float2 anchor = float2_half;
        entity parent = canvas;
        byte layer = 2;
        color fcolor = color_cyan;
        color ocolor = color_black;

        dbg_texture_generated = spawn_element2(world, prefab_element_frame, canvas, parent, position, size, tsize, anchor, layer, fcolor, ocolor);

        zox_log("Spawned [dbg_texture_generated]");
    }

    spawn_sound_from_file_index(world, prefab_sound, 0);
}
*/
