// tst: furniture_models, the bed and door models are bigger than a block (doc/plans/multi_block_vox.md)
//  - generated at depth 6 (two blocks a side): the bed's voxels span 2 x 1 x 1 blocks, the door's 1 x 2 x 1,
//      both from the cube's low corner (the base block), with all their colours used
//  - run: bash bsh/tst.sh furniture_models
entity zox_tst_furniture_bed = 0, zox_tst_furniture_door = 0;

byte zox_tst_furniture_models_ready(ecs* world) {
    if (!zox_tst_furniture_bed) {
        zox_tst_furniture_bed = spawn_model_lods_generated(world, 0, "tst_bed", vox_type_bed, (color) { 140, 90, 50, 255 }, 6, 666);
        zox_tst_furniture_door = spawn_model_lods_generated(world, 0, "tst_door", vox_type_door, (color) { 120, 80, 45, 255 }, 6, 667);
        return 0;
    }
    zox_tst_waiting = "the bed and door models to generate";
    return zox_getv(zox_tst_furniture_bed, GenerateModel) == zox_generate_model_end &&
        zox_getv(zox_tst_furniture_door, GenerateModel) == zox_generate_model_end;
}

// a model's solid voxels: their count, their bounds (max is the last voxel + 1), which colours are used
static int zox_tst_furniture_bounds(ecs* world, entity model, int3* min, int3* max, int* kinds) {
    const byte depth = zox_getv(model, NodeDepth);
    const int length = octree_size(depth);
    const VoxelNode* node = zox_get(model, VoxelNode);
    byte used[256] = { 0 };
    int solid = 0;
    *min = (int3) { length, length, length };
    *max = int3_zero;
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        const byte voxel = getv_VoxelNode(node, depth, (byte3) { x, y, z });
        if (!voxel) {
            continue;
        }
        solid++;
        used[voxel] = 1;
        min->x = x < min->x ? x : min->x; min->y = y < min->y ? y : min->y; min->z = z < min->z ? z : min->z;
        max->x = x + 1 > max->x ? x + 1 : max->x; max->y = y + 1 > max->y ? y + 1 : max->y; max->z = z + 1 > max->z ? z + 1 : max->z;
    }
    *kinds = 0;
    for (int i = 1; i < 256; i++) {
        *kinds += used[i];
    }
    return solid;
}

void zox_tst_furniture_models_check(ecs* world) {
    const int block = octree_size(6) / 2;
    int3 min, max;
    int kinds;
    int solid = zox_tst_furniture_bounds(world, zox_tst_furniture_bed, &min, &max, &kinds);
    zox_log("[tst] [metric] bed [%i] voxels from [%ix%ix%i] to [%ix%ix%i], [%i] colours used of [%i]", solid, min.x, min.y, min.z,
        max.x, max.y, max.z, kinds, zox_get(zox_tst_furniture_bed, ColorRGBs)->length);
    zox_tst_check(solid > 0 && !min.x && !min.y && !min.z, "the bed doesn't start at its base block's corner");
    zox_tst_check(max.x == block * 2 && max.z == block, "the bed is [%i] by [%i] voxels, not 2 blocks by 1", max.x, max.z);
    zox_tst_check(max.y <= block && max.y >= block / 4, "the bed is [%i] voxels tall, not within a block", max.y);
    zox_tst_check(kinds == 5, "the bed uses [%i] colours, not its wood, dark wood, sheet, pillow and blanket", kinds);
    solid = zox_tst_furniture_bounds(world, zox_tst_furniture_door, &min, &max, &kinds);
    zox_log("[tst] [metric] door [%i] voxels from [%ix%ix%i] to [%ix%ix%i], [%i] colours used", solid, min.x, min.y, min.z,
        max.x, max.y, max.z, kinds);
    zox_tst_check(solid > 0 && !min.x && !min.y, "the door doesn't start at its base block's corner");
    zox_tst_check(max.x == block && max.y == block * 2, "the door is [%i] by [%i] voxels, not 1 block by 2", max.x, max.y);
    zox_tst_check(min.z > 0 && max.z < block && max.z - min.z <= block / 2, "the door is [%i] to [%i] deep, not thin inside its block", min.z, max.z);
    zox_tst_check(kinds == 3, "the door uses [%i] colours, not its wood, dark wood and handle", kinds);
}
