void build_vox_bricks(VoxelNode *voctree, byte depth, byte2 voxels, byte black) {

    byte vlength = powers_of_two_byte[depth];
    if (vlength == 1) {
        set_VoxelNode(voctree, depth, byte3_zero, black, 0);
        return;
    }

    byte3 size = byte3_single(vlength);
    byte3 pos;

    // parameters you can tweak
    byte brick_w     = vlength / 4; // 8;
    byte brick_h     = vlength / 8; // 4;
    byte brick_d     = vlength / 4;
    byte mortar      = vlength > 8 ? 1 : 0;
    byte chip_chance = vlength / 8; // 5; // percent

    if (brick_w < 2) brick_w = 2;
    if (brick_h < 2) brick_h = 2;
    if (brick_d < 2) brick_d = 2;
    if (!mortar) mortar = 1;
    if (!chip_chance) chip_chance = 1;

    for (pos.y = 0; pos.y < size.y; pos.y++) {
        // which “row” of bricks am I in?
        byte div = (brick_h + mortar);
        byte row = byte_div(pos.y, div);
        byte stagger = row & 1;  // every other row is offset

        for (pos.x = 0; pos.x < size.x; pos.x++) {
            // compute x with stagger
            int sx = pos.x + (stagger ? (brick_w / 2 + mortar) : 0);

            for (pos.z = 0; pos.z < size.z; pos.z++) {
                // brick mask
                byte in_x = (sx % (brick_w + mortar)) < brick_w;
                byte in_y = (pos.y % (brick_h + mortar)) < brick_h;
                byte in_z = (pos.z % (brick_d + mortar)) < brick_d;

                if (in_x && in_y && in_z) {

                    // random chip at edges
                    byte chip = (rand() % 100) < chip_chance
                    && (
                        (sx % (brick_w + mortar) < 2) ||
                        (sx % (brick_w + mortar) >= brick_w-2) ||
                        (pos.z % (brick_d + mortar) < 1) ||
                        (pos.z % (brick_d + mortar) >= brick_d-1)
                    );

                    if (chip) {

                        set_VoxelNode(voctree, depth, pos, black, 0);

                    } else {

                        byte voxel = voxels.x + rand() % (voxels.y - voxels.x);
                        set_VoxelNode(voctree, depth, pos, voxel, 0);

                    }
                } else {

                    // mortar
                    set_VoxelNode(voctree, depth, pos, black, 0);

                }
            }
        }
    }
}



// Testing
/*set_VoxelNode(voctree, 0, byte3_zero, voxels.x, 0);
set_VoxelNode(voctree, 1, byte3_zero, black, 0);
zox_log("1 voctree opened? %i", !is_closed_VoxelNode(voctree));
zox_log("1 voctree ptr? %i", voctree->ptr != NULL);
return;
*/