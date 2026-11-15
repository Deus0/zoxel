// block data: a byte for the few voxels that need one (a bed's or door's direction, a door's state), on their chunk
//  - BlockDatas holds { position, value } for those only, empty for most chunks (no memory)
//  - written with the voxel (VoxelNodeUpdate's data, voxel_update_queue_system), under the chunk's octree lock
//  - saved after the octree as a section (save_block_datas): an id byte, a 2 byte count, 4 bytes an entry; a chunk with
//    none writes nothing, so its file is as it was, and an older file (no section) loads with none
#define block_datas_section 1

// a voxel's block data, 0 if none
static inline byte get_block_data(const BlockDatas* datas, byte3 position) {
    for (int i = 0; i < datas->length; i++) {
        if (byte3_equals(datas->value[i].position, position)) {
            return datas->value[i].value;
        }
    }
    return 0;
}

// sets a voxel's block data, 0 removes it
static inline void set_block_data(BlockDatas* datas, byte3 position, byte value) {
    for (int i = 0; i < datas->length; i++) {
        if (byte3_equals(datas->value[i].position, position)) {
            if (value) {
                datas->value[i].value = value;
            } else {
                datas->value[i] = datas->value[datas->length - 1];
                resize_BlockDatas(datas, datas->length - 1);
            }
            return;
        }
    }
    if (value) {
        add_to_BlockDatas(datas, (BlockData) { position, value });
    }
}

byte save_block_datas(FILE* out, const BlockDatas* datas) {
    if (!datas || !datas->length) {
        return 1;
    }
    const byte section = block_datas_section;
    const ushort count = (ushort) datas->length;
    if (fwrite(&section, 1, 1, out) != 1 || fwrite(&count, sizeof(ushort), 1, out) != 1) {
        return 0;
    }
    for (int i = 0; i < count; i++) {
        const byte entry[4] = { datas->value[i].position.x, datas->value[i].position.y, datas->value[i].position.z,
            datas->value[i].value };
        if (fwrite(entry, 1, 4, out) != 4) {
            return 0;
        }
    }
    return 1;
}

// reads the sections after a chunk's octree: its block datas, none at the file's end (older files)
byte load_block_datas(FILE* in, BlockDatas* datas) {
    byte section;
    while (fread(&section, 1, 1, in) == 1) {
        if (section != block_datas_section) {
            zox_logw("[load_block_datas] unknown chunk section [%i]", section);
            return 0;
        }
        ushort count = 0;
        if (fread(&count, sizeof(ushort), 1, in) != 1) {
            return 0;
        }
        for (int i = 0; i < count; i++) {
            byte entry[4];
            if (fread(entry, 1, 4, in) != 4) {
                return 0;
            }
            if (datas) {
                set_block_data(datas, (byte3) { entry[0], entry[1], entry[2] }, entry[3]);
            }
        }
    }
    return 1;
}
