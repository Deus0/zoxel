// TODO: Use Entity Colors here instead of globals
/*zox_sys2(ToggleStateSystem) {
    zox_sys_begin()
    zox_sys_in(ToggleState);
    zox_sys_in(ToggleStateDirty);
    zox_sys_in(ElementOutlineColor);
    zox_sys_out(OutlineColor);
    zox_sys_out(GenerateTexture);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ToggleState, state);
        zox_sys_i(ToggleStateDirty, dirty);
        zox_sys_i(ElementOutlineColor, idle_outline);
        zox_sys_o(OutlineColor, outline);
        zox_sys_o(GenerateTexture, generate);
        if (dirty->value == zox_dirty_active) {
            // brightness->value = state->value ? ui_active_brightness : ui_default_brightness;
            outline->value = state->value ? active_outline : idle_outline->value;
            generate->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(ToggleStateSystem);
*/
