// Buildings: boxes of blocks, written in a building's own frame and turned to face its way, filled into the chunks
//  they cross (like model nodes, build_home writes a home as boxes)
//  - its frame: x across, y up from its floor, z out of its front; a box's corners inclusive
//  - later boxes write over earlier ones, a box of air (0) carves (a doorway)
//  - pieces: blocks with block data (a door's, a bed's parts, chunks3/fun/block_data.c), written after the boxes
#define building_boxes_max 96
#define building_pieces_max 16

typedef struct {
    int3 min;
    int3 max;
    byte block;
} BuildBox;

typedef struct {
    int3 at;
    byte block;
    byte data;
} BuildPiece;

typedef struct {
    int2 center;
    byte facing;    // its front faces: 0 +z, 1 +x, 2 -z, 3 -x
    int floor;
    BuildBox boxes[building_boxes_max];
    byte count;
    BuildPiece pieces[building_pieces_max];
    byte pieces_count;
} Building;

// its frame to the world, an offset from its middle
static inline int2 build_turn(byte facing, int x, int z) {
    switch (facing & 3) {
        case 1: return (int2) { z, -x };
        case 2: return (int2) { -x, -z };
        case 3: return (int2) { -z, x };
        default: return (int2) { x, z };
    }
}

// the world to its frame, an offset from its middle
static inline int2 build_unturn(byte facing, int2 offset) {
    switch (facing & 3) {
        case 1: return (int2) { -offset.y, offset.x };
        case 2: return (int2) { -offset.x, -offset.y };
        case 3: return (int2) { offset.y, -offset.x };
        default: return offset;
    }
}

// a box in its frame
static inline void build_box(Building* b, int3 a, int3 c, byte block) {
    if (b->count >= building_boxes_max) {
        return;
    }
    const int2 p = build_turn(b->facing, a.x, a.z), q = build_turn(b->facing, c.x, c.z);
    b->boxes[b->count++] = (BuildBox) {
        { b->center.x + int_min(p.x, q.x), b->floor + int_min(a.y, c.y), b->center.y + int_min(p.y, q.y) },
        { b->center.x + int_max(p.x, q.x), b->floor + int_max(a.y, c.y), b->center.y + int_max(p.y, q.y) },
        block
    };
}

// the block direction (chunks3/fun/block_data.c) that turns a model's axis to a way in the world (x, z)
static inline byte build_direction(int3 axis, int2 way) {
    for (byte d = 0; d < 4; d++) {
        const int3 o = block_direction_offset(d, axis);
        if (o.x == way.x && o.z == way.y) {
            return d;
        }
    }
    return 0;
}

// a sized block (a bed, a door: items/fun/multi_block.c) at base, world blocks, turned to direction: its base holds the
//  block and its direction, the rest of its box the filler and its offset back to the base
static inline void build_multi_at(Building* b, int3 base, int3 size, byte direction, byte block, byte filler) {
    if (!block || !filler) {
        return;
    }
    for (int x = 0; x < size.x; x++)
    for (int y = 0; y < size.y; y++)
    for (int z = 0; z < size.z; z++) {
        if (b->pieces_count >= building_pieces_max) {
            return;
        }
        const int3 o = block_direction_offset(direction, (int3) { x, y, z });
        const byte is_base = !x && !y && !z;
        b->pieces[b->pieces_count++] = (BuildPiece) { int3_add(base, o), is_base ? block : filler,
            is_base ? 1 + direction : block_offset_encode(int3_scale(o, -1)) };
    }
}

// a sized block at a point in its frame, its model's axis turned to a way in its frame
static inline void build_multi(Building* b, int3 at, int3 size, int3 axis, int2 way, byte block, byte filler) {
    const int2 p = build_turn(b->facing, at.x, at.z);
    const int3 base = { b->center.x + p.x, b->floor + at.y, b->center.y + p.y };
    build_multi_at(b, base, size, build_direction(axis, build_turn(b->facing, way.x, way.y)), block, filler);
}

// a box's voxels in a chunk along an axis (a voxel is hmultiplier blocks at lods, a thin box keeps a voxel), 0 if none
static inline byte build_span(int min, int max, int chunk_start, byte hmultiplier, byte length, int* from, int* to) {
    *from = int_max(floor_div(min - chunk_start, hmultiplier), 0);
    *to = int_min(floor_div(max - chunk_start, hmultiplier), length - 1);
    return *from <= *to;
}

// a building's pieces in a chunk at full detail, with their block data (none at lods: their models don't show there)
static inline void build_pieces_in_chunk(VoxelNode* voctree, BlockDatas* datas, byte depth, int3 chunk_block, byte length, const Building* b) {
    for (byte i = 0; i < b->pieces_count; i++) {
        const BuildPiece* piece = &b->pieces[i];
        const int3 p = int3_sub(piece->at, chunk_block);
        if (p.x < 0 || p.y < 0 || p.z < 0 || p.x >= length || p.y >= length || p.z >= length) {
            continue;
        }
        set_clean_VoxelNode(voctree, depth, int3_to_byte3(p), piece->block);
        set_block_data(datas, int3_to_byte3(p), piece->data);
    }
}

// fills a building's boxes into a chunk (its first block chunk_block), its pieces too at full detail
static inline void build_in_chunk(VoxelNode* voctree, BlockDatas* datas, byte depth, int3 chunk_block, byte hmultiplier, byte length, const Building* b) {
    for (byte i = 0; i < b->count; i++) {
        const BuildBox* box = &b->boxes[i];
        int3 from, to;
        if (!build_span(box->min.x, box->max.x, chunk_block.x, hmultiplier, length, &from.x, &to.x) ||
            !build_span(box->min.y, box->max.y, chunk_block.y, hmultiplier, length, &from.y, &to.y) ||
            !build_span(box->min.z, box->max.z, chunk_block.z, hmultiplier, length, &from.z, &to.z)) {
            continue;
        }
        byte3 position;
        for (int x = from.x; x <= to.x; x++) {
            for (int y = from.y; y <= to.y; y++) {
                for (int z = from.z; z <= to.z; z++) {
                    position = (byte3) { x, y, z };
                    set_clean_VoxelNode(voctree, depth, position, box->block);
                }
            }
        }
    }
    if (hmultiplier == 1 && datas) {
        build_pieces_in_chunk(voctree, datas, depth, chunk_block, length, b);
    }
}

// sets a block in a chunk at a world height, if it's in this chunk (a column at a time: town walls, roads)
static inline byte town_set_block(VoxelNode* voctree, byte depth, byte3* position, int global_y, int chunk_y, byte hmultiplier, byte length, byte value) {
    if (global_y < chunk_y) {
        return 0;
    }
    const int y = (global_y - chunk_y) / hmultiplier;
    if (y >= length) {
        return 0;
    }
    position->y = y;
    set_clean_VoxelNode(voctree, depth, *position, value);
    return 1;
}
