// block data: bytes for the few voxels that need them (a bed's direction, a door's state, later a sign's text, a
//  chest's id for its own file), on their chunk
//  - BlockDatas packs its entries in one buffer: position (3 bytes), length (1), data (length, 1 to 255 bytes); empty
//    for most chunks (no memory), no pointers inside (moves and copies are a memcpy)
//  - written with the voxel (VoxelNodeUpdate's data, voxel_update_queue_system), under the chunk's octree lock
//  - saved after the octree as a section (save_block_datas): an id byte, a 2 byte size, the buffer; a chunk with none
//    writes nothing; more section ids can follow later
#define block_datas_section 1
#define block_data_header 4

// a voxel's entry's offset in the buffer, -1 if it has none
static inline int find_block_data(const BlockDatas* datas, byte3 position) {
    for (int i = 0; i + block_data_header <= datas->length; i += block_data_header + datas->value[i + 3]) {
        const byte* entry = &datas->value[i];
        if (entry[0] == position.x && entry[1] == position.y && entry[2] == position.z) {
            return i;
        }
    }
    return -1;
}

// a voxel's data and its length, NULL if none
static inline const byte* get_block_data_bytes(const BlockDatas* datas, byte3 position, byte* length) {
    const int i = find_block_data(datas, position);
    *length = i >= 0 ? datas->value[i + 3] : 0;
    return i >= 0 ? &datas->value[i + block_data_header] : NULL;
}

// sets a voxel's data, length 0 removes it
void set_block_data_bytes(BlockDatas* datas, byte3 position, const byte* data, byte length) {
    const int i = find_block_data(datas, position);
    if (i >= 0) {
        // removed: the entries after it moved down
        const int size = block_data_header + datas->value[i + 3];
        memmove(&datas->value[i], &datas->value[i + size], datas->length - i - size);
        resize_BlockDatas(datas, datas->length - size);
    }
    if (!length) {
        return;
    }
    const int at = datas->length;
    resize_BlockDatas(datas, at + block_data_header + length);
    byte* entry = &datas->value[at];
    entry[0] = position.x;
    entry[1] = position.y;
    entry[2] = position.z;
    entry[3] = length;
    memcpy(&entry[block_data_header], data, length);
}

// a voxel's one byte of data (a direction, a state), 0 if none
static inline byte get_block_data(const BlockDatas* datas, byte3 position) {
    byte length;
    const byte* data = get_block_data_bytes(datas, position, &length);
    return data && length ? data[0] : 0;
}

// sets a voxel's one byte of data, 0 removes it
static inline void set_block_data(BlockDatas* datas, byte3 position, byte value) {
    set_block_data_bytes(datas, position, &value, value ? 1 : 0);
}

int count_block_datas(const BlockDatas* datas) {
    int count = 0;
    for (int i = 0; i + block_data_header <= datas->length; i += block_data_header + datas->value[i + 3]) {
        count++;
    }
    return count;
}

byte save_block_datas(FILE* out, const BlockDatas* datas) {
    if (!datas || !datas->length) {
        return 1;
    }
    const byte section = block_datas_section;
    const ushort size = (ushort) datas->length;
    return fwrite(&section, 1, 1, out) == 1 && fwrite(&size, sizeof(ushort), 1, out) == 1 &&
        fwrite(datas->value, 1, size, out) == size;
}

// reads the sections after a chunk's octree: its block datas, none if the file ends there
byte load_block_datas(FILE* in, BlockDatas* datas) {
    byte section;
    while (fread(&section, 1, 1, in) == 1) {
        if (section != block_datas_section) {
            zox_logw("[load_block_datas] unknown chunk section [%i]", section);
            return 0;
        }
        ushort size = 0;
        if (fread(&size, sizeof(ushort), 1, in) != 1) {
            return 0;
        }
        if (!datas) {
            fseek(in, size, SEEK_CUR);
            continue;
        }
        resize_BlockDatas(datas, size);
        if (fread(datas->value, 1, size, in) != size) {
            resize_BlockDatas(datas, 0);
            return 0;
        }
    }
    return 1;
}

// a placed bed's or door's turn, its base's block data: 1 + its direction (quarter turns about up, 0 to 3)
//  - its fillers' data: 1 + their offset back to their base (block_offset_encode), so each finds its base exactly
#define block_offset_range 2    // fillers within 2 blocks of their base each way (blocks up to 3 a side)

// a direction's turn about up
static inline float4 block_direction_rotation(byte direction) {
    return quaternion_from_axis_angle(float3_up, (direction & 3) * (float) M_PI * 0.5f);
}

// an offset turned by a direction (as its model turns), in whole blocks
static inline int3 block_direction_offset(byte direction, int3 offset) {
    const float3 turned = float4_rotate_float3(block_direction_rotation(direction), int3_to_float3(offset));
    return (int3) { (int) roundf(turned.x), (int) roundf(turned.y), (int) roundf(turned.z) };
}

// a facing's direction: the turn that points a model's length (its +x) most along where it faces, so a bed lies
//  away from its placer (its yaw alone laid it sideways)
static inline byte block_direction_from_forward(float3 forward) {
    byte best = 0;
    float best_dot = -2;
    for (byte d = 0; d < 4; d++) {
        const float3 length = float4_rotate_float3(block_direction_rotation(d), (float3) { 1, 0, 0 });
        const float dot = length.x * forward.x + length.z * forward.z;
        if (dot > best_dot) {
            best_dot = dot;
            best = d;
        }
    }
    return best;
}

static inline byte block_direction_from_rotation(float4 rotation) {
    return block_direction_from_forward(float4_rotate_float3(rotation, float3_forward));
}

// where a placer looks: its camera's ray, else its body's facing
static inline byte block_direction_of_placer(ecs* world, entity placer) {
    const entity camera = zox_get_link(world, placer, CameraLink);
    if (zox_valid(camera) && zox_has(camera, RaycastNormal)) {
        return block_direction_from_forward(zox_getv(camera, RaycastNormal));
    }
    return zox_has(placer, Rotation3D) ? block_direction_from_rotation(zox_getv(placer, Rotation3D)) : 0;
}

static inline byte block_offset_encode(int3 offset) {
    const int r = block_offset_range, w = r * 2 + 1;
    return (byte) (1 + (offset.x + r) + (offset.y + r) * w + (offset.z + r) * w * w);
}

static inline int3 block_offset_decode(byte data) {
    const int r = block_offset_range, w = r * 2 + 1;
    const int i = data - 1;
    return (int3) { i % w - r, (i / w) % w - r, i / (w * w) - r };
}
