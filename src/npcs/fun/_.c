
static inline byte can_have_characters(ecs* world, entity chunk) {
    return zox_getv(chunk, NpcSpawnZone);
}
