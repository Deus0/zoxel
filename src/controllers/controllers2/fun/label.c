int get_label_player_character2D(ecs *world, entity player, char buffer[], int buffer_size, int buffer_index) {
    entity character = zox_getv(player, CharacterLink);
    if (!zox_valid(character) || !zox_has(character, Position2)) return buffer_index;
    const float2 position2 = zox_getv(character, Position2);
    buffer_index += snprintf(buffer + buffer_index, buffer_size - buffer_index, "player [%ix%i]\n", (int) position2.x, (int) position2.y);
    return buffer_index;
}

uint get_label_character2_player(ecs *world, entity player, char *buffer, uint size, uint index) {
    if (!player) {
        index += snprintf(buffer + index, size - index, "! invalid player\n");
        return index;
    }
    zox_geter(player, CharacterLink, characterLink);
    if (!zox_valid(characterLink->value)) {
        index += snprintf(buffer + index, size - index, "[%s] has no character\n", zox_get_name(player));
        return index;
    }
    zox_geter_value(player, CameraLink, entity, camera);
    zox_geter_value(characterLink->value, Position2, float2, position2);
    zox_geter_value(camera, Position3D, float3, camera_position3);
    index += snprintf(buffer + index, size - index, "player [%s]:\n", zox_get_name(player));
    index += snprintf(buffer + index, size - index, "   - is controlling [%s]\n",  zox_get_name(characterLink->value));
    index += snprintf(buffer + index, size - index, "   - is looking from [%s]\n", zox_get_name(camera));
    index += snprintf(buffer + index, size - index, "   - located at [%fx%f]\n", position2.x, position2.y);
    index += snprintf(buffer + index, size - index, "   - camera at [%fx%f]x%f]\n", camera_position3.x, camera_position3.y, camera_position3.z);
    return index;
}