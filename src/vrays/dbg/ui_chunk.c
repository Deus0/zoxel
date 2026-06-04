extern entity get_linked_character(ecs*, entity);

uint debug_ui_chunk3(ecs *world, const char* label, entity chunk, char *buffer, uint size, uint index) {
    if (!zox_valid(chunk)) {
        index += snprintf(buffer + index, size - index, "![%s] Chunk\n", label);
        return index;
    }
    zox_geter_value(chunk, ChunkPosition, int3, cposition);
    zox_geter_value(chunk, NodeDepth, byte, ndepth);
    zox_geter_value(chunk, RenderDepth, byte, rdepth);
    zox_geter_value(chunk, RenderDisabled, byte, disabled);
    zox_geter_value(chunk, RenderDistance, byte, rdistance);
    index += snprintf(buffer + index, size - index, "[%s] Chunk [%s]\n", label, zox_get_name(chunk));
    index += snprintf(buffer + index, size - index, "   + pos [%ix%ix%i]\n", cposition.x, cposition.y, cposition.z);
    index += snprintf(buffer + index, size - index, "   + ndepth [%i]\n", ndepth);
    index += snprintf(buffer + index, size - index, "   + rdepth [%i]\n", rdepth);
    index += snprintf(buffer + index, size - index, "   + rvisible [%i]\n", !disabled);
    index += snprintf(buffer + index, size - index, "   + rdistance [%i]\n", rdistance);
    zox_geter(chunk, ChunkNeighbors, neighbors);
    for (byte i = 0; i < 6; i++) {
        entity n = neighbors->value[i];
        if (!zox_valid(n) || !zox_has(n, RenderDepth)) {
            continue;
        }
        zox_geter_value(n, RenderDepth, byte, nrdepth);
        index += snprintf(buffer + index, size - index, "   + n [%i] rdepth [%i]\n", i, nrdepth);
    }
    return index;
}

uint debug_ui_raycast_chunk3(ecs *world, entity player, char *buffer, uint size, uint index) {
    if (!player) {
        index += snprintf(buffer + index, size - index, "Invalid player.\n");
        return index;
    }
    entity character = get_linked_character(world, player);
    if (!zox_valid(character)) {
        index += snprintf(buffer + index, size - index, "[%s] has no character\n", zox_get_name(player));
        return index;
    }
    index = debug_ui_chunk3(world, "selected", dbg_chunk, buffer, size, index);
    zox_geter(character, RaycastVoxelData, data);
    if (data->chunk != dbg_chunk) {
        index = debug_ui_chunk3(world, "highlighted", data->chunk, buffer, size, index);
    }
    if (!zox_valid(dbg_chunk)) {
        if (zox_valid(data->chunk)) {
            index += snprintf(buffer + index, size - index, "Press [f] to select a chunk!\n");
        } else {
            index += snprintf(buffer + index, size - index, "Aim at a chunk!\n");
        }
    }
    return index;
}
