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
static inline void update_light_emission(
    ecs* world,
    entity manager,
    entity* cached_manager,
    byte* emission)
{
    if (*cached_manager == manager) {
        return;
    }
    *cached_manager = manager;
    memset(emission, 0, zox_light_solidity_length);
    if (!zox_valid(manager) || !zox_has(manager, BlockLinks)) {
        return;
    }
    const BlockLinks* blocks = zox_get(manager, BlockLinks);
    const int length = blocks->length < zox_light_solidity_length ? blocks->length : zox_light_solidity_length;
    for (int j = 0; j < length; j++) {
        const entity block = blocks->value[j];
        emission[j] = zox_valid(block) && zox_has(block, BlockLight) ? zox_getv(block, BlockLight) : 0;
    }
}
