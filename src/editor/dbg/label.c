uint zox_dbg_ui_hierarchy(ecs *world, entity e, char *buffer, uint size, uint index, byte dig) {
    byte estimated_line = 64;
    dig++;
    entity children[layouts2_children_capacity];
    uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
    for (uint j = 0; j < children_length; j++) {
        entity e2 = children[j];

        if (!e2) {
            continue;
        }

        if (zox_has(e2, EditorElement)) {
            continue;
        }

        for (byte j = 0; j < dig; j++) {
            index += snprintf(buffer + index, size - index, " ");
        }

        index += snprintf(buffer + index, size - index, "- [%s]\n", zox_get_name(e2));

        if (index + estimated_line >= size) {
            return index;
        }

        index = zox_dbg_ui_hierarchy(world, e2, buffer, size, index, dig);
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
