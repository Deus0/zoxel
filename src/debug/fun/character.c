uint zox_dbg_label_character_links(ecs *world, entity player, char *buffer, uint size, uint index) {
    entity e = zox_getv(player, CharacterLink);
    if (!zox_valid(e)) {
        return index;
    }
    float3 position = zox_getv(e, Position3D);
    float3 euler = zox_getv(e, Euler);
    entity camera = zox_getv(e, CameraLink);
    entity chunk = zox_getv(e, ChunkLink);
    entity tunk = zox_valid(chunk) ? zox_getv(chunk, TunkLink) : 0;
    entity region = zox_valid(tunk) ? zox_getv(tunk, RegionLink) : 0;
    // Character
    index += snprintf(buffer + index, size - index, "Character [%s]\n", zox_get_name(e));
    index += snprintf(buffer + index, size - index, " - Region [%s]\n", zox_get_name(region));
    index += snprintf(buffer + index, size - index, " - Tunk [%s]\n", zox_get_name(tunk));
    index += snprintf(buffer + index, size - index, " - Chunk [%s]\n", zox_get_name(chunk));
    index += snprintf(buffer + index, size - index, " - Position [%.1fx%.1fx%.1f]\n", position.x, position.y, position.z);
    index += snprintf(buffer + index, size - index, " - Euler [%.1fx%.1fx%.1f]\n", euler.x, euler.y, euler.z);
    if (zox_valid(camera)) {
        index += snprintf(buffer + index, size - index, "Camera [%s]\n", zox_get_name(camera));
        zox_geter_value(camera, Position3D, float3, camera_position)
        zox_geter_value(camera, Euler, float3, camera_euler)
        index += snprintf(buffer + index, size - index, " - pos [%.1fx%.1fx%.1f]\n", camera_position.x, camera_position.y, camera_position.z);
        index += snprintf(buffer + index, size - index, " - eul [%.1fx%.1fx%.1f]\n", camera_euler.x, camera_euler.y, camera_euler.z);
    }
    // int3 voxel_position = zox_getv(character, BlockPosition);
    // positions
    // index += snprintf(buffer + index, size - index, " - in [%ix%ix%i]\n", chunk_position.x, chunk_position.y, chunk_position.z);
    // chunk
    // zox_geter_value(chunk, ChunkPosition, int3, chunk_chunk_position)
    // index += snprintf(buffer + index, size - index, " - at [%ix%ix%i]\n", chunk_chunk_position.x, chunk_chunk_position.y, chunk_chunk_position.z);
    return index;
}
