uint zox_dbg_ui_hierarchy(ecs *world, entity e, char *buffer, uint size, uint index, byte dig) {
    byte estimated_line = 64;

    if (!zox_has(e, Children)) {
        return index;
    }

    dig++;

    zox_geter(e, Children, children);
    for (int i = 0; i < children->length; i++) {
        entity child = children->value[i];

        if (!child) {
            continue;
        }

        if (zox_has(child, EditorElement)) {
            continue;
        }

        for (byte j = 0; j < dig; j++) {
            index += snprintf(buffer + index, size - index, " ");
        }

        index += snprintf(buffer + index, size - index, "- [%s]\n", zox_get_name(child));

        if (index + estimated_line >= size) {
            return index;
        }

        index = zox_dbg_ui_hierarchy(world, child, buffer, size, index, dig);
    }

    return index;
}

uint zox_dbg_ui_canvas(ecs *world, entity player, char *buffer, uint size, uint index) {
    if (!player) {
        index += snprintf(buffer + index, size - index, "Invalid player\n");
        return index;
    }

    zox_geter_value(player, CanvasLink, entity, canvas);

    return zox_dbg_ui_hierarchy(world, canvas, buffer, size, index, 0);
}

// sets update function
void set_prefab_debug_label(ecs *world, DebugLabelEvent value) {
    if (!zox_valid(prefab_game_debug_label)) {
        zox_log_error("prefab_game_debug_label has not spawned yet");
        return;
    }
    zox_set(prefab_game_debug_label, DebugLabelData, { value })
}