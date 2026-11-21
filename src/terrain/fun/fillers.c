// Fillers: the rest of a sized block's box (a bed, a door, a street lamp), solid, drawn by its base
//  (items/fun/multi_block.c places and removes them)

// the realm's filler voxel value, 0 if it has none
byte realm_filler_voxel(ecs* world, entity realm) {
    const BlockLinks* blocks = zox_valid(realm) ? zox_get(realm, BlockLinks) : NULL;
    for (int i = 0; blocks && i < blocks->length; i++) {
        if (zox_valid(blocks->value[i]) && zox_has(blocks->value[i], BlockFiller)) {
            return i + 1;
        }
    }
    return 0;
}

// a sized block's filler voxel value: the realm's filler passing light as its base does (a door's stops it while it's
//  shut, block_light_pass_open), the plain one otherwise, 0 if none
byte block_filler_voxel(ecs* world, entity realm, entity base) {
    const byte open = zox_valid(base) && zox_has(base, BlockLightPass) && zox_getv(base, BlockLightPass) == block_light_pass_open;
    const BlockLinks* blocks = zox_valid(realm) ? zox_get(realm, BlockLinks) : NULL;
    for (int i = 0; open && blocks && i < blocks->length; i++) {
        const entity b = blocks->value[i];
        if (zox_valid(b) && zox_has(b, BlockFiller) && zox_getv(b, BlockLightPass) == block_light_pass_open) {
            return i + 1;
        }
    }
    return realm_filler_voxel(world, realm);
}
