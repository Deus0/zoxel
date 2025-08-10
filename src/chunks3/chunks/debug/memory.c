uint debug_ui_memory(
    ecs *world,
    const entity player,
    char *buffer,
    const uint size,
    uint index
) {
    index += snprintf(buffer + index, size - index, "Terrain Chunks [%i]\n", zox_stats_terrain_chunks);

    index += snprintf(buffer + index, size - index, "Nodes [%i]\n", zox_stats_nodes);

    index += snprintf(buffer + index, size - index, "Arrayds [%i]\n", zox_stats_arrayds_mallocs);
    index += snprintf(buffer + index, size - index, "   Arrayds2 [%i]\n", zox_stats_arrayds_exists);

    return index;
}