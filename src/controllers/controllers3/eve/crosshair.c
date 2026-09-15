void player_state_crosshair(ecs* world, entity player, byte state) {
    byte dbg_log = 0;
    byte is_spawn = state == zox_player_state_play_begin;
    byte is_destroy =
        state == zox_player_state_pause_begin ||
        state == zox_player_state_respawn_begin;
    if (!is_destroy && !is_spawn) {
        return;
    }
    entity canvas = zox_get_link(world, player, CanvasLink);
    if (!zox_valid(canvas)) {
        return;
    }
    entity ui = zox_get_child_by_id(
        world,
        canvas,
        zox_id(Crosshair));
    if (is_spawn && !zox_valid(ui)) {
            int2 size = int2_single(crosshair_size * ui_scale);
            entity crosshair = spawn_ui(
                world,
                prefab_crosshair,
                canvas,
                float2_half,
                int2_zero,
                size,
                size);
            zox_set_unique_name(crosshair, "crosshair");
        if (dbg_log) {
            zox_log("Spawned [Crosshair] for [%s]",
                zox_getn(player));
        }
    } else if (is_destroy && zox_valid(ui)) {
        if (dbg_log) {
            zox_log("Destroying [Crosshair] [%s] for [%s]",
                zox_getn(ui),
                zox_getn(player));
        }
        zox_delete(ui);
    }
}