// a realm's water blocks by their level (doc/tech/water.md Levels): the source (BlockWater, 8) and flowing 7 to 1
typedef struct {
    byte voxel[zox_water_level_source + 1];     // a level's voxel (block index), 0 none
    byte level[256];                            // a voxel's level, 0 not water
} WaterLevels;

// fills levels from the realm's blocks, 0 if it has no water
static inline byte water_levels_get(ecs* world, entity realm, WaterLevels* levels) {
    memset(levels, 0, sizeof(WaterLevels));
    if (!zox_valid(realm) || !zox_has(realm, BlockLinks)) {
        return 0;
    }
    const BlockLinks* blocks = zox_get(realm, BlockLinks);
    for (int i = 0; i < blocks->length && i < 255; i++) {
        const entity block = blocks->value[i];
        if (!zox_valid(block) || !zox_has(block, BlockWaterLevel)) {
            continue;
        }
        const byte level = zox_getv(block, BlockWaterLevel);
        if (level && level <= zox_water_level_source) {
            levels->voxel[level] = i + 1;
            levels->level[i + 1] = level;
        }
    }
    return levels->voxel[zox_water_level_source] != 0;
}

// is a voxel water, any level
static inline byte voxel_is_water(const WaterLevels* levels, int voxel) {
    return voxel > 0 && voxel < 256 && levels->level[voxel];
}
