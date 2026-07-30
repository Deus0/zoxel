uint debug_ui_memory(
    ecs *world,
    entity player,
    char *buffer,
    uint size,
    uint index
) {
    index += snprintf(buffer + index, size - index, "Arrayds [%i]\n", zox_stats_arrayds_mallocs);
    index += snprintf(buffer + index, size - index, "   Arrayds2 [%i]\n", zox_stats_arrayds_exists);
    return index;
}