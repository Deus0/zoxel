uint get_label_realm_colors(ecs *world, const entity player, char *buffer, const uint size, uint index) {
    if (!zox_valid(player)) {
        index += snprintf(buffer + index, size - index, "! invalid player\n");
        return index;
    }
    entity game = zox_get_link(world, player, GameLink);
    // const RealmLink* realmLink = zox_get(gameLink->value, RealmLink);
    entity realm = zox_get_link(world, game, RealmLink);
    if (!zox_valid(realm) || !zox_has(realm, Colors)) {
        index += snprintf(buffer + index, size - index, "[%s] has invalid realm\n", zox_get_name(player));
        return index;
    }
    const Colors* colors = zox_get(realm, Colors);
    const Seed* seed = zox_get(realm, Seed);
    index += snprintf(buffer + index, size - index, "[%s]\n", zox_get_name(realm));
    index += snprintf(buffer + index, size - index, " - total [%i]\n", colors->length);
    index += snprintf(buffer + index, size - index, " - seed [%lu]\n", seed->value);
    for (int i = 0; i < colors->length; i++) {
        const color c = colors->value[i];
        index += snprintf(buffer + index, size - index, " - [%i] [%i.%i.%i]\n", i, c.r, c.g, c.b);
    }
    return index;
}