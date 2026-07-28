void build_vox_heights(VoxelNode* voxels, byte depth, byte2 range, byte max_height) {
    short length = octree_size(depth);
    byte3 position;
    for (position.x = 0; position.x < length; position.x++) {
        for (position.z = 0; position.z < length; position.z++) {
            byte height = rand_range(0, max_height);
            for (position.y = 0; position.y < height; position.y++) {
                byte value = rand_range(range.x, range.y);
                set_voxel_safe(voxels, depth, position, value);
            }
        }
    }
}

// NOTE: This is quite laggy atm due to the large amounts of rand()s
// NOTE: Simulates more natural random placement
void build_vox_scatter(VoxelNode* voxels, byte depth, byte2 range, byte max_height, uint count) {
    byte length = octree_size(depth);
    byte heights[length][length];
    memset(heights, 0, sizeof(heights));
    for (uint i = 0; i < count; i++) {
        byte x = rand() % length;
        byte z = rand() % length;
        if (heights[x][z] < max_height) {
            heights[x][z]++;
        }
    }
    byte3 position;
    for (position.x = 0; position.x < length; position.x++) {
        for (position.z = 0; position.z < length; position.z++) {
            byte height = heights[position.x][position.z];
            for (position.y = 0; position.y < height; position.y++) {
                byte value = rand_range(range.x, range.y);
                set_voxel_safe(voxels, depth, position, value);
            }
        }
    }
}

// fill_octree(voxels, 0, depth);  // clear chunk
/*for (int j = 0; j < rubble_count; j++) {
    position.x = rand() % length;
    position.z = rand() % length;
    for (int k = 0; k < length; k++) {
        position.y = k;
        if (getv_VoxelNode(voxels, depth, position) == 0) {
            break;
        }
    }
    if (position.y > rubble_height) {
        continue;
    }
    byte value = voxel_range.x + rand() % (voxel_range.y - voxel_range.x);
    set_VoxelNode(voxels, depth, position, value);
}*/