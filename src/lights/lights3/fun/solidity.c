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
    zox_geter(manager, BlockLinks, blocks);
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
