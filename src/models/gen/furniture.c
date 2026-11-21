// Furniture models, bigger than a block: a model's cube is two blocks a side (a lamp's four) (doc/plans/multi_block_vox.md)
//  - a block is half the model's length, u is a 16th of a block: shapes are in u, so any depth works
//  - both sit in the cube's low corner: the base block is x, y, z from 0, the model grows from it

// voxels from min up to max (not including max), clipped to the model
static inline void furniture_fill(VoxelNode* node, byte depth, int3 min, int3 max, byte voxel) {
    const int size = octree_size(depth);
    for (int x = min.x < 0 ? 0 : min.x; x < max.x && x < size; x++)
    for (int y = min.y < 0 ? 0 : min.y; y < max.y && y < size; y++)
    for (int z = min.z < 0 ? 0 : min.z; z < max.z && z < size; z++) {
        set_VoxelNode(node, depth, (byte3) { x, y, z }, voxel);
    }
}

#define furniture_wood 1
#define furniture_wood_dark 2
#define furniture_sheet 3
#define furniture_pillow 4
#define furniture_blanket 5
#define furniture_metal 6
#define lamp_metal_dark 1
#define lamp_metal 2
#define lamp_glow 3

// a bed, 2 x 1 x 1 blocks, lying along x: the pillow end is the base block
void build_vox_bed(VoxelNode* node, byte depth) {
    const int length = octree_size(depth), block = length / 2, u = block / 16 ? block / 16 : 1;
    // legs
    for (byte i = 0; i < 4; i++) {
        const int x = i & 1 ? length - 2 * u : 0, z = i & 2 ? block - 2 * u : 0;
        furniture_fill(node, depth, (int3) { x, 0, z }, (int3) { x + 2 * u, 2 * u, z + 2 * u }, furniture_wood_dark);
    }
    // frame, headboard, footboard
    furniture_fill(node, depth, (int3) { 0, 2 * u, 0 }, (int3) { length, 5 * u, block }, furniture_wood);
    furniture_fill(node, depth, (int3) { 0, 2 * u, 0 }, (int3) { u, 12 * u, block }, furniture_wood_dark);
    furniture_fill(node, depth, (int3) { length - u, 2 * u, 0 }, (int3) { length, 8 * u, block }, furniture_wood_dark);
    // mattress, pillow, blanket
    furniture_fill(node, depth, (int3) { u, 5 * u, u }, (int3) { length - u, 8 * u, block - u }, furniture_sheet);
    furniture_fill(node, depth, (int3) { 2 * u, 8 * u, 3 * u }, (int3) { 7 * u, 10 * u, block - 3 * u }, furniture_pillow);
    furniture_fill(node, depth, (int3) { 9 * u, 8 * u, u }, (int3) { length - u, 9 * u, block - u }, furniture_blanket);
    // the blanket's folded edge
    furniture_fill(node, depth, (int3) { 9 * u, 9 * u, u }, (int3) { 11 * u, 10 * u, block - u }, furniture_sheet);
}

// a door, 1 x 2 x 1 blocks, thin in z, on its block's front edge (its handle and braces just inside it): it swings
//  from its hinge at the corner into its own block (vodes/doors door_on_spawned, door_hinge_z)
void build_vox_door(VoxelNode* node, byte depth) {
    const int length = octree_size(depth), block = length / 2, u = block / 16 ? block / 16 : 1;
    const int z0 = 2 * u, z1 = 4 * u;
    // planks, every other one darker
    for (int x = 0; x < block; x += 4 * u) {
        furniture_fill(node, depth, (int3) { x, 0, z0 }, (int3) { x + 4 * u, length, z1 },
            (x / (4 * u)) & 1 ? furniture_wood_dark : furniture_wood);
    }
    // braces across it, proud of the planks on both faces
    for (byte i = 0; i < 3; i++) {
        const int y = i == 0 ? 3 * u : i == 1 ? block - u : length - 5 * u;
        furniture_fill(node, depth, (int3) { 0, y, z0 - u }, (int3) { block, y + 2 * u, z1 + u }, furniture_wood_dark);
    }
    // a handle through it, a block up
    furniture_fill(node, depth, (int3) { block - 4 * u, block - 5 * u, z0 - 2 * u }, (int3) { block - 2 * u, block - 3 * u, z1 + 2 * u },
        furniture_metal);
}

// a street lamp, 1 x 3 x 1 blocks, in a cube four blocks a side: a plinth, a pole, a lantern of glowing glass, a cap
void build_vox_lamp(VoxelNode* node, byte depth) {
    const int block = octree_size(depth) / 4, u = block / 16 ? block / 16 : 1;
    furniture_fill(node, depth, (int3) { 4 * u, 0, 4 * u }, (int3) { 12 * u, 2 * u, 12 * u }, lamp_metal_dark);
    furniture_fill(node, depth, (int3) { 6 * u, 2 * u, 6 * u }, (int3) { 10 * u, 4 * u, 10 * u }, lamp_metal);
    furniture_fill(node, depth, (int3) { 7 * u, 4 * u, 7 * u }, (int3) { 9 * u, 40 * u, 9 * u }, lamp_metal_dark);
    // the lantern: a plate, glass with a post at each corner, a cap and a knob
    furniture_fill(node, depth, (int3) { 5 * u, 40 * u, 5 * u }, (int3) { 11 * u, 41 * u, 11 * u }, lamp_metal);
    furniture_fill(node, depth, (int3) { 5 * u, 41 * u, 5 * u }, (int3) { 11 * u, 46 * u, 11 * u }, lamp_glow);
    for (byte i = 0; i < 4; i++) {
        const int x = i & 1 ? 10 * u : 5 * u, z = i & 2 ? 10 * u : 5 * u;
        furniture_fill(node, depth, (int3) { x, 41 * u, z }, (int3) { x + u, 46 * u, z + u }, lamp_metal_dark);
    }
    furniture_fill(node, depth, (int3) { 4 * u, 46 * u, 4 * u }, (int3) { 12 * u, 47 * u, 12 * u }, lamp_metal_dark);
    furniture_fill(node, depth, (int3) { 6 * u, 47 * u, 6 * u }, (int3) { 10 * u, 48 * u, 10 * u }, lamp_metal_dark);
}
