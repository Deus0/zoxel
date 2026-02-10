extern entity spawn_element_texture(
    ecs *world,
    const entity canvas,
    const entity source_texture,
    const int2 position,
    const int2 size
);

extern entity get_linked_canvas(ecs*, entity);

entity tilemap_ui = 0;

void spawn_tilemap_ui(ecs *world, int32_t keycode) {
    if (keycode == SDLK_9) {
        if (tilemap_ui) {
            zox_log("- removing tilemap ui")
            zox_delete(tilemap_ui)
            tilemap_ui = 0;
        } else {
            // conjure the canvas
            entity realm = local_realm;

            zox_geter(realm, GameLink, gameLink);
            zox_geter(gameLink->value, PlayerLinks, players);

            entity player = players->value[0];

            entity canvas = get_linked_canvas(world, player);
            // our logic stuff
            zox_log("+ spawning tilemap ui")

            int2 size = (int2) { 320, 320 };
            const entity tilemap = zox_get_value(realm, TilemapLink)
            int2 position = (int2) { 8, 8 };
            tilemap_ui = spawn_element_texture(world, canvas, tilemap, position, size);
        }
        spawn_sound_from_file_index(world, prefab_sound, 0);
    }
}
