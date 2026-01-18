// contains air!
void get_block_collisions(ecs *world, const BlockLinks *voxels, byte *collisions) {
    collisions[0] = 0;    // air
    for (int i = 0; i < voxels->length; i++) {
        const entity block = voxels->value[i];
        if (zox_valid(block)) {
            collisions[i + 1] = zox_gett_value(block, BlockCollider) != zox_block_air;
        } else {
            collisions[i + 1] = 0;
        }
    }
}
