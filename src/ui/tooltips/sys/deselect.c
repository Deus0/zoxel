// NOTE: The tooltip will shut down when no more uis are linked to it.
// NOTE: This might of been when the selected dies too
zox_sys2(TooltipDeselectSystem) {
    byte dbg_log = zox_log_tooltips;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(TextData);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TextData, data);
        // NOTE: The dirty state delays the race conditions
        if (!data->length) {
            continue;
        }
        // check linked, if link found, continue
        if (zox_is_linked_any(world, e, TooltipLink)) {
            continue;
        }
        // Clear text
        resize_TextData(data, 0);
        zox_add(e, Dirty);
        if (dbg_log) {
            zox_log("Tooltip is shutting down [%s]",
                zox_getn(e));
        }
    }
} zox_sys_end(TooltipDeselectSystem);
