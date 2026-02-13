void toggle_inspector(ecs *world, int32_t keycode) {
    // conjure the canvas
    /*const entity realm = local_realm;
    zox_geter(realm, GameLink, gameLink)
    zox_geter(gameLink->value, PlayerLinks, players)
    const entity player = players->value[0];*/


    // our logic stuff
    if (keycode != SDLK_u) {
        return;
    }
    entity player = dbg_player;
    zox_geter(player, CanvasLink, canvasLink);
    toggle_ui(world, canvasLink->value, &inspector, &spawn_inspector);
    spawn_sound_from_file_index(world, prefab_sound, 0);
}
