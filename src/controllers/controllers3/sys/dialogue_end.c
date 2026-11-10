// Run from DialoguetreeRun
zox_sys2(DialogueEndSystem) {
    zox_sys_world();
    zox_sys_begin();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        entity ui = zox_get_link(world, e, DialogueUI);
        if (zox_valid(ui)) {
            zox_delete(ui);
        }
        const entity speaker_a = get_speaker_a(world, e);
        const entity speaker_b = get_speaker_b(world, e);
        if (zox_valid(speaker_b)) {
            unfollow(world, speaker_b);
        }
        if (zox_valid(speaker_a)) {
            entity player = zox_get_link(world, speaker_a, PlayerLink);
            if (zox_valid(player)) {
                zox_set(player, PlayerState, { zox_player_state_dialogue_end });
                entity canvas = zox_get_link(world, player, CanvasLink);
                entity windows[zox_children_capacity];
                uint length = zox_get_children_by_id(
                    world,
                    canvas,
                    windows,
                    zox_children_capacity,
                    zox_id(Window));
                for (int j = 0; j < length; j++) {
                    entity window = windows[j];
                    set_children_by_id_byte(
                        world,
                        window,
                        zox_id(RenderDisabled),
                                            0);
                }
            }
        }

    }
} zox_sys_end(DialogueEndSystem);
