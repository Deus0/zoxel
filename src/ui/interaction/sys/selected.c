/*zox_sys2(ElementSelectedSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SelectState);
    zox_sys_out(Brightness);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SelectState, state);
        zox_sys_o(Brightness, brightness);
        byte is_dragging = zox_has(e, DraggableState) ? zox_gett_value(e, DraggableState) : 0;
        byte is_active = zox_has(e, ActiveState) ? zox_gett_value(e, ActiveState) : 0;
        if (is_active) {
            brightness->value = ui_active_brightness;
        } else if (is_dragging) {
            brightness->value = ui_dragging_brightness;
        } else if (state->value) {
            brightness->value = ui_selected_brightness;
        } else {
            brightness->value = ui_default_brightness;
        }
    }
} zox_sys_end(ElementSelectedSystem);*/

zox_sys2(ElementSelectedBrighterSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SelectState);
    zox_sys_out(Brightness);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SelectState, state);
        zox_sys_o(Brightness, brightness);
        if (state->value == zox_state_select_active) {
            brightness->value = ui_selected_brightness;
            if (dbg_log) {
                zox_log("[%s] Brightness Set (Selected) [%f]", zox_get_name(e), brightness->value);
            }
        } else if (state->value == zox_state_deselect_active) {
            brightness->value = ui_default_brightness;
            if (dbg_log) {
                zox_log("[%s] Brightness Set (Idle) [%f]", zox_get_name(e), brightness->value);
            }
        }
    }
} zox_sys_end(ElementSelectedBrighterSystem);

zox_sys2(ElementSelectedFillColorSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SelectState);
    zox_sys_in(ElementFillColor);
    zox_sys_in(SelectedFillColor);
    zox_sys_out(FillColor);
    zox_sys_out(GenerateTexture);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SelectState, state);
        zox_sys_i(ElementFillColor, idle_color);
        zox_sys_i(SelectedFillColor, selected_color);
        zox_sys_o(FillColor, fill);
        zox_sys_o(GenerateTexture, generate);
        if (state->value == zox_state_select_active) {
            if (!color_equals(fill->value, selected_color->value)) {
                fill->value = selected_color->value;
                generate->value = zox_generate_texture_run;
                if (dbg_log) {
                    zox_log("New Fill Color (Selected) [%s] is [%ix%ix%ix%i]", zox_get_name(e), fill->value.r, fill->value.g, fill->value.b, fill->value.a);
                }
            }
        } else if (state->value == zox_state_deselect_active) {
            if (!color_equals(fill->value, idle_color->value)) {
                fill->value = idle_color->value;
                generate->value = zox_generate_texture_run;
                if (dbg_log) {
                    zox_log("New Fill Color (Idle) [%s] is [%ix%ix%ix%i]", zox_get_name(e), fill->value.r, fill->value.g, fill->value.b, fill->value.a);
                }
            }
        }
    }
} zox_sys_end(ElementSelectedFillColorSystem);

// NOTE: Updates texture outline color when selected
// NOTE: Disabled if ActiveState is Enabled
zox_sys2(ElementSelectedOutlineColorSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SelectState);
    zox_sys_in(ElementOutlineColor);
    zox_sys_in(SelectedOutlineColor);
    zox_sys_out(OutlineColor);
    zox_sys_out(GenerateTexture);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SelectState, state);
        zox_sys_i(ElementOutlineColor, idle_color);
        zox_sys_i(SelectedOutlineColor, selected_color);
        zox_sys_o(OutlineColor, outline);
        zox_sys_o(GenerateTexture, generate);
        if (zox_has(e, ActiveState)) {
            if (zox_getv(e, ActiveState)) {
                continue;
            }
        }
        if (state->value == zox_state_select_active) {
            if (!color_equals(outline->value, selected_color->value)) {
                outline->value = selected_color->value;
                generate->value = zox_generate_texture_run;
                if (dbg_log) {
                    zox_log("New Outline Color (Selected) [%s] is [%ix%ix%ix%i]", zox_get_name(e), outline->value.r, outline->value.g, outline->value.b, outline->value.a);
                }
            }
        } else if (state->value == zox_state_deselect_active) {
            if (!color_equals(outline->value, idle_color->value)) {
                outline->value = idle_color->value;
                generate->value = zox_generate_texture_run;
                if (dbg_log) {
                    zox_log("New Outline Color (Idle) [%s] is [%ix%ix%ix%i]", zox_get_name(e), outline->value.r, outline->value.g,  outline->value.b, outline->value.a);
                }
            }
        }
    }
} zox_sys_end(ElementSelectedOutlineColorSystem);
