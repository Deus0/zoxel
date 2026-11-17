// NOTE: When start / stop terrain loading, spawn / destroy the load ui
//  - on 3D huds the view is dark while it's up (huds3_dark): a headset sees black and the label, not the world loading
void game_state_load(ecs* world, entity game, byte state) {
    byte is_spawn = state == zox_game_load_begin;
    byte is_destroy = state == zox_game_load_end;
    if (state == zox_game_state_the_end || state == zox_game_none) {
        huds3_dark = 0;     // left mid load
    }
    if (state == zox_game_load_start && zox_huds3D) {
        huds3_dark = 1;     // fading to black (huds3_dark_system) as the menu goes, before loading begins
    }
    if (!is_spawn && !is_destroy) {
        return;
    }
    huds3_dark = is_spawn && zox_huds3D;   // lit again, faded in, at load end
    iter it2 = zox_children(world, game);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            entity player = it2.entities[j];
            if (!zox_has(player, Player)) {
                continue;
            }
            entity canvas = zox_get_link(world, player, CanvasLink);
            if (!canvas) {
                continue;
            }
            entity load_ui = zox_get_child_by_id(
                world,
                canvas,
                zox_id(LoadUI));
            if (is_spawn && !zox_valid(load_ui)) {
                spawn_load_ui(
                    world,
                    player,
                    canvas);
            } else if (is_destroy && zox_valid(load_ui)) {
                zox_delete(load_ui);
            }
        }
    }
}