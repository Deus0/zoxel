// animation system
zox_sys2(ElementActiveSystem) {
    zox_sys_begin()
    zox_sys_in(ActiveState);
    zox_sys_in(ActiveStateDirty);
    zox_sys_out(OutlineColor);
    zox_sys_out(Brightness);
    zox_sys_out(GenerateTexture);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ActiveState, state);
        zox_sys_i(ActiveStateDirty, dirty);
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
} zox_sys_end(ElementActiveSystem);