// contains air!
void get_block_collisions(ecs *world, const BlockLinks *voxels, byte *collisions) {
    collisions[0] = 0;    // air
    for (int i = 0; i < voxels->length; i++) {
        entity block = voxels->value[i];
        if (zox_valid(block)) {
            collisions[i + 1] = zox_getv(block, BlockCollider) != zox_block_air;
        } else {
            collisions[i + 1] = 0;
        }
    }
}

// No Air
void get_block_sounds(ecs *world, const BlockLinks *blocks, entity *sounds) {
    for (int i = 0; i < blocks->length; i++) {
        entity block = blocks->value[i];
        if (zox_valid(block)) {
            sounds[i] = zox_getv(block, BlockSound);
        } else {
            sounds[i] = 0;
        }
    }
}
