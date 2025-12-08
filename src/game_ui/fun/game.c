extern entity spawn_menu_start(
    ecs *world,
    const entity player,
    const entity canvas
);

void spawn_players_start_ui(ecs *world) {
    zox_logv("> spawn_players_start_ui [%i]", players_playing)
    for (int i = 0; i < players_playing; i++) {
        const entity player = zox_players[i];
        zox_geter_value(player, CanvasLink, entity, canvas);
        zox_logv("    - player [%s] - canvas[%s]", zox_get_name(player), zox_get_name(canvas));
        spawn_menu_start(world, player, canvas);
    }
}