// animation system
zox_sys2(ToggleStateSystem) {
    zox_sys_begin()
    zox_sys_in(ToggleState);
    zox_sys_in(ToggleStateDirty);
    zox_sys_out(OutlineColor);
    zox_sys_out(Brightness);
    zox_sys_out(GenerateTexture);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ToggleState, state);
        zox_sys_i(ToggleStateDirty, dirty);
        zox_sys_o(OutlineColor, outline);
        zox_sys_o(Brightness, brightness);
        zox_sys_o(GenerateTexture, generate);

        if (dirty->value != zox_dirty_active) {
            continue;
        }

        brightness->value = state->value ? ui_active_brightness : ui_default_brightness;
        outline->value = state->value ? color_purple : default_outline_color_frame;
        generate->value = zox_dirty_trigger;
    }
} zox_sys_end(ToggleStateSystem);