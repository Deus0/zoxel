// an element made active or not: its outline (element_active_link_event removes ActiveStateDirty after)
// an observer on ActiveStateDirty (non fragmenting, removed after)
void element_active_event(iter* it) {
    zox_sys_begin();
    zox_sys_in(ActiveState);
    zox_sys_in(ElementOutlineColor);
    zox_sys_in(ActiveColor);
    zox_sys_out(OutlineColor);
    zox_sys_out(GenerateTexture);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ActiveState, state);
        zox_sys_i(ElementOutlineColor, idle_outline);
        zox_sys_i(ActiveColor, acolor);
        zox_sys_o(OutlineColor, outline);
        zox_sys_o(GenerateTexture, generate);
        outline->value = state->value ? acolor->value : idle_outline->value;
        generate->value = zox_generate_texture_run;
    }
}
