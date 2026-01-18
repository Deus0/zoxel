byte only_single_npc = 0;
entity single_npc = 0;

uint debug_ui_character_position(
    ecs *world,
    const entity character,
    char *buffer,
    const uint size,
    uint index
) {
    if (!zox_valid(character)) {
        index += snprintf(buffer + index, size - index, "No Character\n");
        return index;
    }

    index += snprintf(buffer + index, size - index, "Name [%s]\n", zox_get_name(character));

    zox_geter_value(character, Position3D, float3, position);
    index += snprintf(buffer + index, size - index, "Position [%f.1x%f.1x%f.1]\n", position.x, position.y, position.z);

    zox_geter_value(character, TerrainLink, entity, terrain);
    index += snprintf(buffer + index, size - index, "Terrain [%s]\n", zox_get_name(terrain));

    zox_geter_value(terrain, BlockScale, float, terrain_scale);
    zox_geter_value(terrain, NodeDepth, byte, terrain_depth);
    const byte3 max_chunk_size = byte3_single(powers_of_two[terrain_depth]);
    const int3 positionv = positionf_to_positionv(position, terrain_scale);
    const int3 positionc = positionv_to_positionc(positionv, max_chunk_size);

    index += snprintf(buffer + index, size - index, "Position Voxel [%ix%ix%i]\n", positionv.x, positionv.y, positionv.z);
    index += snprintf(buffer + index, size - index, "Position Chunk [%ix%ix%i]\n", positionc.x, positionc.y, positionc.z);

    /*zox_geter_value(character, ChunkLink, entity, chunk);
    index += snprintf(buffer + index, size - index, "NPC [%s]\n", zox_get_name(character));*/

    return index;
}

uint debug_ui_single_npc(
    ecs *world,
    const entity player,
    char *buffer,
    const uint size,
    uint index
) {
    index += snprintf(buffer + index, size - index, "NPCs [%li]\n", zox_stats_characters);

    index += snprintf(buffer + index, size - index, "NPC:\n");
    index = debug_ui_character_position(world, single_npc, buffer, size, index);

    index += snprintf(buffer + index, size - index, "Player:\n");
    zox_geter_value(player, CharacterLink, entity, player_character);
    index = debug_ui_character_position(world, player_character, buffer, size, index);

    return index;
}