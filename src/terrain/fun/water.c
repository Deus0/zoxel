// a realm's liquids by their level (doc/tech/water.md Levels): each a source (level 8: BlockWater, BlockOil) and its
//  flowing blocks 7 to 1 (BlockWaterFlowing, linked to their source: LiquidLink)
#define zox_liquids_max 4
typedef struct {
    byte voxel[zox_liquids_max][zox_water_level_source + 1];    // a liquid's level's voxel (block index), 0 none
    byte level[256];                                            // a voxel's level, 0 not a liquid
    byte liquid[256];                                           // a voxel's liquid, its index in voxel
    byte count;
} WaterLevels;

// fills levels from the realm's blocks, 0 if it has no liquids
static inline byte water_levels_get(ecs* world, entity realm, WaterLevels* levels) {
    memset(levels, 0, sizeof(WaterLevels));
    if (!zox_valid(realm) || !zox_has(realm, BlockLinks)) {
        return 0;
    }
    const BlockLinks* blocks = zox_get(realm, BlockLinks);
    entity sources[zox_liquids_max];
    // the sources first, a liquid each, then each flowing block under its source's (water's without a link)
    for (int i = 0; i < blocks->length && i < 255; i++) {
        const entity block = blocks->value[i];
        if (levels->count < zox_liquids_max && zox_valid(block) && zox_has(block, BlockWaterLevel) &&
            zox_getv(block, BlockWaterLevel) == zox_water_level_source)
        {
            sources[levels->count] = block;
            levels->voxel[levels->count][zox_water_level_source] = i + 1;
            levels->level[i + 1] = zox_water_level_source;
            levels->liquid[i + 1] = levels->count;
            levels->count++;
        }
    }
    for (int i = 0; i < blocks->length && i < 255; i++) {
        const entity block = blocks->value[i];
        if (!zox_valid(block) || !zox_has(block, BlockWaterLevel)) {
            continue;
        }
        const byte level = zox_getv(block, BlockWaterLevel);
        if (!level || level >= zox_water_level_source) {
            continue;
        }
        const entity source = zox_get_link(world, block, LiquidLink);
        byte liquid = 0;
        for (byte l = 0; l < levels->count; l++) {
            if (sources[l] == source) {
                liquid = l;
            }
        }
        levels->voxel[liquid][level] = i + 1;
        levels->level[i + 1] = level;
        levels->liquid[i + 1] = liquid;
    }
    return levels->count != 0;
}

// is a voxel a liquid (water, oil), any level
static inline byte voxel_is_water(const WaterLevels* levels, int voxel) {
    return voxel > 0 && voxel < 256 && levels->level[voxel];
}
