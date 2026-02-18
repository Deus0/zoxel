void toggle_life_terrain(ecs *world, int32_t keycode) {
    if (keycode == SDLK_k) {

        zox_log("> terrain death toggling");

        entity realm = local_realm;

        if (!zox_valid(realm)) {
            zox_log_error("Invalid [local_realm]")
            return;
        }

        entity terrain = local_terrain;

        if (!zox_valid(terrain)) {
            zox_log("Spawning Terrain on [%s]", zox_get_name(realm));

            spawn_terrain_on_realm(world, realm);
        } else {
            zox_log("Destroying Terrain [%s]", zox_get_name(terrain));

            zox_delete(terrain);
        }
    }
}
