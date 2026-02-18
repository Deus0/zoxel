extern entity spawn_element_texture(ecs*, entity, entity, int2, int2);
extern entity get_linked_canvas(ecs*, entity);
extern entity local_terrain;

entity tilemap_ui = 0;

void spawn_tilemap_ui(ecs *world, int32_t keycode) {

    int2 size = (int2) { 320, 320 };
    int2 position = (int2) { 8, 8 };

    if (keycode != SDLK_9) {
        return;
    }

    if (tilemap_ui) {
        zox_log("- removing tilemap ui")
        zox_delete(tilemap_ui);
        tilemap_ui = 0;
    } else {

        entity player = dbg_player;
        if (!zox_valid(player)) {
            return;
        }

        entity terrain = local_terrain;
        if (!zox_valid(terrain)) {
            return;
        }

        entity canvas = get_linked_canvas(world, player);
        zox_geter_value(terrain, TilemapLink, entity, tilemap);

        // our logic stuff
        zox_log("+ spawning tilemap ui")

        tilemap_ui = spawn_element_texture(world, canvas, tilemap, position, size);
    }
    spawn_sound_from_file_index(world, prefab_sound, 0);
}
