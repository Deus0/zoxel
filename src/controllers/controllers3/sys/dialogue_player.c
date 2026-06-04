// NOTE: This handles player dialogue state changes
zox_sys2(PlayerDialogueSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CharacterLink);
    zox_sys_in(CanvasLink);
    zox_sys_out(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(CharacterLink, character);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_o(PlayerState, state);
        if (!zox_valid(canvas->value)) {
            zox_logw("Canvas is missing from Player");
            continue;
        }
        if (state->value == zox_player_state_dialogue_begin) {
            state->value = zox_player_state_dialogue_active;
            trigger_canvas_half_fade(world, canvas->value, pause_fade_time, pause_fade_alpha, 1);
            if (local_mouse) {
                zox_set(local_mouse, MouseLock, { 0 });
            }
            zox_set(character->value, DisableMovement, { 1 });

        } else if (state->value == zox_player_state_dialogue_end) {
            state->value = zox_player_state_playing;
            trigger_canvas_half_fade(world, canvas->value, pause_fade_time, pause_fade_alpha, 0);
            if (local_mouse) {
                zox_set(local_mouse, MouseLock, { 1 });
            }
            zox_set(character->value, DisableMovement, { 0 });
        }
    }
} zox_sys_end(PlayerDialogueSystem);
