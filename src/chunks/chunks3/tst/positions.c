// tst: chunk_positions, world <-> block <-> chunk <-> local position maths
//  - every conversion is checked against one plain reference: floor_div(floor(position / scale), length)
//  - random positions plus every chunk edge, both sides of 0 (negative coordinates are where these break)
//  - chunk * length + local == block, for every block
//  - run: bash bsh/tst.sh chunk_positions

static inline int zox_tst_floor_div(int x, int d) {
    int q = x / d;
    if ((x % d != 0) && ((x < 0) != (d < 0))) {
        q--;
    }
    return q;
}

static inline int zox_tst_floor_mod(int x, int d) {
    return x - zox_tst_floor_div(x, d) * d;
}

uint zox_tst_positions_seed = 77;

static inline uint zox_tst_positions_rand() {
    uint x = zox_tst_positions_seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    zox_tst_positions_seed = x;
    return x;
}

byte zox_tst_chunk_positions_ready(ecs* world) {
    (void) world;
    return 1;
}

// one real position, all conversions, returns errors (logs the first)
static inline int zox_tst_position_check(float position, float scale, byte depth, int* logged) {
    const int length = octree_size(depth);
    const int block = (int) floor(position / scale);
    const int chunk = zox_tst_floor_div(block, length);
    int errors = 0;
    const int block_a = real_position_to_block_position1(position, scale);
    const int chunk_a = real_position_to_chunk_position1(position, length, scale);
    const int3 chunk_b = real_position_to_chunk_position((float3) { position, -position, position * 0.5f }, length, scale);
    const int3 chunk_c = block_position_to_chunk_position((int3) { block, block, block }, depth);
    const int3 back = chunk_position_to_block_position((int3) { chunk, chunk, chunk }, depth);
    const byte3 local = block_position_to_local_position((int3) { block, block, block }, depth, depth);
    // the other axes of chunk_b use their own references
    const int chunk_neg = zox_tst_floor_div((int) floor(-position / scale), length);
    const int chunk_half = zox_tst_floor_div((int) floor(position * 0.5f / scale), length);
    const int local_expected = zox_tst_floor_mod(block, length);
    if (block_a != block ||
        chunk_a != chunk ||
        chunk_b.x != chunk || chunk_b.y != chunk_neg || chunk_b.z != chunk_half ||
        chunk_c.x != chunk ||
        back.x != chunk * length ||
        local.x != local_expected ||
        chunk * length + local.x != block)
    {
        errors++;
        if (!(*logged)++) {
            zox_loge("[tst] position [%f] scale [%f] depth [%i]: block [%i] expected [%i], chunk1 [%i] chunk3 [%ix%ix%i] expected [%ix%ix%i], from block [%i], back [%i], local [%i] expected [%i]",
                position, scale, depth,
                block_a, block,
                chunk_a,
                chunk_b.x, chunk_b.y, chunk_b.z, chunk, chunk_neg, chunk_half,
                chunk_c.x, back.x,
                local.x, local_expected);
        }
    }
    return errors;
}

void zox_tst_chunk_positions_check(ecs* world) {
    (void) world;
    const float scales[] = { 1.0f, 0.5f, 0.25f, 2.0f };
    const byte depths[] = { 3, 4, 5 };
    int errors = 0;
    int checks = 0;
    int logged = 0;
    for (int s = 0; s < 4; s++) {
        for (int d = 0; d < 3; d++) {
            const float scale = scales[s];
            const byte depth = depths[d];
            const int length = octree_size(depth);
            // every block edge and middle across chunks -3 .. 3
            for (int block = -3 * length; block <= 3 * length; block++) {
                const float edge = block * scale;
                const float middle = (block + 0.5f) * scale;
                errors += zox_tst_position_check(edge, scale, depth, &logged);
                errors += zox_tst_position_check(middle, scale, depth, &logged);
                checks += 2;
            }
            // random, far from 0
            for (int i = 0; i < 2000; i++) {
                const float position = ((int) (zox_tst_positions_rand() % 200000) - 100000) * 0.013f * scale;
                errors += zox_tst_position_check(position, scale, depth, &logged);
                checks++;
            }
        }
    }
    zox_log("[tst] position conversions checked [%i], wrong [%i]", checks, errors);
    zox_tst_check(!errors, "[%i] of [%i] position conversions are wrong", errors, checks);
    // local position at a lower chunk depth halves per depth
    const byte3 low = block_position_to_local_position((int3) { 15, 7, -1 }, 4, 3);
    zox_tst_check(low.x == 7 && low.y == 3 && low.z == 7,
        "local position at a lower depth [%ix%ix%i], expected [7x3x7]", low.x, low.y, low.z);
    // floor_div itself
    int floor_errors = 0;
    for (int x = -100; x <= 100; x++) {
        for (int dv = 1; dv <= 17; dv++) {
            floor_errors += floor_div(x, dv) != zox_tst_floor_div(x, dv);
        }
    }
    zox_tst_check(!floor_errors, "floor_div wrong [%i] times", floor_errors);
}

void zox_tsts_chunks3_positions() {
    zox_tst_add("chunk_positions", zox_tst_chunk_positions_ready, zox_tst_chunk_positions_check, 30);
}
