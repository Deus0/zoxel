#define zox_light_solidity_length 255

// Caches if each block stops light, indexed by voxel - 1
// NOTE: Only rebuilds when the chunks block manager (realm) changes
static inline void update_light_solidity(
    ecs* world,
    entity manager,
    entity* cached_manager,
    byte* solidity)
{
    if (*cached_manager == manager) {
        return;
    }
    *cached_manager = manager;
    if (!zox_valid(manager) ||
        !zox_has(manager, BlockLinks))
    {
        return;
    }
    const BlockLinks* blocks = zox_get(manager, BlockLinks);
    const int length = blocks->length < zox_light_solidity_length ?
        blocks->length :
        zox_light_solidity_length;
    for (int j = 0; j < length; j++) {
        entity block = blocks->value[j];
        solidity[j] = zox_valid(block) &&
            zox_has(block, BlockLightPass) ?
                !zox_getv(block, BlockLightPass) :
                1;
    }
}

// Caches the light each block gives off (BlockLight, 0 none), indexed by voxel - 1, like solidity
//  - and where from, blocks from its voxel (BlockLightOffset: a street lamp's head), its voxel for most
static inline void update_light_emission(
    ecs* world,
    entity manager,
    entity* cached_manager,
    byte* emission,
    byte3* emission_offset)
{
    if (*cached_manager == manager) {
        return;
    }
    *cached_manager = manager;
    memset(emission, 0, zox_light_solidity_length);
    memset(emission_offset, 0, zox_light_solidity_length * sizeof(byte3));
    if (!zox_valid(manager) || !zox_has(manager, BlockLinks)) {
        return;
    }
    const BlockLinks* blocks = zox_get(manager, BlockLinks);
    const int length = blocks->length < zox_light_solidity_length ? blocks->length : zox_light_solidity_length;
    for (int j = 0; j < length; j++) {
        const entity block = blocks->value[j];
        emission[j] = zox_valid(block) && zox_has(block, BlockLight) ? zox_getv(block, BlockLight) : 0;
        emission_offset[j] = emission[j] && zox_has(block, BlockLightOffset) ? zox_getv(block, BlockLightOffset) : byte3_zero;
    }
}

// Caches which blocks are liquids (water), indexed by voxel - 1, like solidity: sunbeams dim in them
static inline void update_light_liquids(
    ecs* world,
    entity manager,
    entity* cached_manager,
    byte* liquids)
{
    if (*cached_manager == manager) {
        return;
    }
    *cached_manager = manager;
    memset(liquids, 0, zox_light_solidity_length);
    if (!zox_valid(manager) || !zox_has(manager, BlockLinks)) {
        return;
    }
    const BlockLinks* blocks = zox_get(manager, BlockLinks);
    const int length = blocks->length < zox_light_solidity_length ? blocks->length : zox_light_solidity_length;
    for (int j = 0; j < length; j++) {
        const entity block = blocks->value[j];
        liquids[j] = zox_valid(block) && zox_has(block, BlockMaterial) &&
            zox_getv(block, BlockMaterial) == zox_block_material_liquid;
    }
}

// a sunbeam's light after passing one liquid block
static inline byte light_dim(byte light, byte min_light) {
    return light > light_water_decay + min_light ? light - light_water_decay : min_light;
}

// light flooding into a voxel, a step from light: less air_decay, and light_water_decay more into a liquid
static inline byte light_flood_decay(byte light, byte voxel, const byte* liquids, byte air_decay, byte min_light) {
    const byte decay = air_decay + (voxel && liquids[voxel - 1] ? light_water_decay : 0);
    return light > decay + min_light ? light - decay : min_light;
}

// the light a sunbeam brings down into pos from above, 0 if none reaches it
//  - a beam is full sunlight in air, light_dim'd in each liquid under it; there's no marker, so it walks up the
//    column (into the chunk above too) while each voxel's light is what the beam would give it, until full sunlight
static inline byte light_beam_above(
    const VoxelNode* vnode,
    const LightNode* lnode,
    const VoxelNode* up_vnode,
    const LightNode* up_lnode,
    byte depth,
    byte3 pos,
    byte min_light,
    const byte* solidity,
    const byte* liquids)
{
    const int length = octree_size(depth);
    byte first = 0;
    byte below_light = 0;
    byte below_liquid = 0;
    for (int y = pos.y + 1; y < 2 * length; y++) {
        const byte up = y >= length;
        const VoxelNode* v = up ? up_vnode : vnode;
        const LightNode* l = up ? up_lnode : lnode;
        if (!l) {
            return 0;
        }
        const byte3 p = { pos.x, up ? y - length : y, pos.z };
        const byte voxel = v ? getv_VoxelNode(v, depth, p) : 0;
        if (voxel && solidity[voxel - 1]) {
            return 0;
        }
        const byte light = getv_LightNode(l, depth, p);
        if (!first) {
            if (light <= min_light) {
                return 0;
            }
            first = light;
        } else if (below_light != (below_liquid ? light_dim(light, min_light) : light)) {
            return 0;
        }
        if (light == sunlight) {
            return first;
        }
        below_light = light;
        below_liquid = voxel && liquids[voxel - 1];
    }
    return 0;
}
