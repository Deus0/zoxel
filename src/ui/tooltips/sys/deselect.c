// NOTE: The tooltip will shut down when no more uis are linked to it.
zox_sys2(TooltipDeselectSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(TextData);
    zox_sys_out(TextDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(TextData, data);
        zox_sys_o(TextDirty, dirty);
        // NOTE: The dirty state delays the race conditions
        if (!data->length || dirty->value) {
            continue;
        }
        // check linked, if link found, continue
        if (zox_is_linked_any(world, e, TooltipLink)) {
            continue;
        }
        // Clear text
        resize_TextData(data, 0);
        dirty->value = zox_dirty_trigger;
        if (dbg_log) {
            zox_log("Tooltip is shutting down [%s]", zox_getn(e));
        }
    }
} zox_sys_end(TooltipDeselectSystem);