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

uint zox_dbg_label_inside_chunk(ecs *world, entity player, char *buffer, uint size, uint index) {
    entity e = zox_getv(player, CharacterLink);
    if (!zox_valid(e)) {
        return index;
    }
    entity chunk = zox_getv(e, ChunkLink);
    entity tunk = zox_valid(chunk) ? zox_getv(chunk, TunkLink) : 0;
    entity region = zox_valid(tunk) ? zox_getv(tunk, RegionLink) : 0;
    // Character
    index += snprintf(buffer + index, size - index, "Character [%s]\n", zox_get_name(e));
    index += snprintf(buffer + index, size - index, " - Region [%s]\n", zox_get_name(region));
    if (!zox_valid(chunk)) {
        return index;
    }
    // chunk
    index += snprintf(buffer + index, size - index, " - Chunk [%s]\n", zox_get_name(chunk));
    byte busy = zox_getv(chunk, Busy);
    byte build = zox_getv(chunk, BuildChunkMesh);
    byte generate = zox_getv(chunk, GenerateChunk);
    int3 position = zox_getv(chunk, ChunkPosition);
    index += snprintf(buffer + index, size - index, " - busy [%i]\n", busy);
    index += snprintf(buffer + index, size - index, " - generate [%i]\n", generate);
    index += snprintf(buffer + index, size - index, " - build [%i]\n", build);
    index += snprintf(buffer + index, size - index, " - at [%ix%ix%i]\n", position.x, position.y, position.z);
    if (!zox_valid(tunk)) {
        return index;
    }
    index += snprintf(buffer + index, size - index, " - Tunk [%s]\n", zox_get_name(tunk));
    byte generate2 = zox_getv(tunk, GenerateTunk);
    index += snprintf(buffer + index, size - index, " - generate2 [%i]\n", generate2);
    return index;
}

uint zox_dbg_label_towns(ecs *world, entity player, char *buffer, uint size, uint index) {
    entity character = zox_getv(player, CharacterLink);
    if (!zox_valid(character)) {
        return index;
    }
    entity chunk = zox_getv(character, ChunkLink);
    if (!zox_valid(chunk)) {
        return index;
    }
    entity tunk = zox_valid(chunk) ? zox_getv(chunk, TunkLink) : 0;
    entity region = zox_valid(tunk) ? zox_getv(tunk, RegionLink) : 0;
    entity terrain = zox_get_parent(world, chunk);
    // Character
    float3 position = zox_getv(character, Position3D);
    float terrain_scale = zox_getv(terrain, BlockScale);
    int3 block_position = real_position_to_block_position(position, terrain_scale);
    index += snprintf(buffer + index, size - index, " - Position [%.1fx%.1fx%.1f]\n", position.x, position.y, position.z);
    index += snprintf(buffer + index, size - index, " - Block Position [%ix%ix%i]\n", block_position.x, block_position.y, block_position.z);
    // index += snprintf(buffer + index, size - index, "Character [%s]\n", zox_get_name(character));
    index += snprintf(buffer + index, size - index, "Region [%s]\n", zox_get_name(region));
    if (!zox_valid(region)) {
        return index;
    }
    entity towns[zox_children_capacity];
    uint towns_length = zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town));
    index += snprintf(buffer + index, size - index, "Total Towns [%i]\n", towns_length);
    for (uint i = 0; i < towns_length; i++) {
        entity town = towns[i];
        zox_geter(town, ZoxName, name);
        lint seed = zox_getv(town, Seed);
        int2 position = zox_getv(town, BlockPosition2);
        byte2 size2 = zox_getv(town, TownSize);
        index += snprintf(buffer + index, size - index, " Town [%s]\n", zox_get_name(town));
        index += snprintf(buffer + index, size - index, " - Name [%s]\n", name->value);
        index += snprintf(buffer + index, size - index, " - Seed [%lu]\n", seed);
        index += snprintf(buffer + index, size - index, " - Position [%ix%i]\n", position.x, position.y);
        index += snprintf(buffer + index, size - index, " - Size [%ix%i]\n", size2.x, size2.y);
        // TODO: Get Homes list too
        // zox_set(e, WallHeight, { wall_height });
        // zox_set(e, WallThickness, { wall_thickness });
    }
    return index;
}
