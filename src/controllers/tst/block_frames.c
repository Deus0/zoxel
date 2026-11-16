// tst: block_frames, a look at the realm's animated blocks (water): every frame of each, side by side
//  - saves tst/blocks/<block>_frames.ppm (magick it to a png to look): a row of its
//      frames 4x, under it its first frame tiled 2 x 2 (seams show there) and its last frame then its first
//      (the loop's join)
//  - checks: each frame differs from the next, the last leads into the first like any two frames do, and a tile's
//      right edge leads into its left (and bottom into top) like any two columns do: it loops and tiles
//  - run: bash bsh/tst.sh block_frames
#define zox_tst_bframes_scale 4

byte zox_tst_block_frames_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_waiting = "the animated blocks' textures to generate";
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const BlockLinks* blocks = zox_get(zox_get_link(world, character, RealmLink), BlockLinks);
    for (int i = 0; i < blocks->length; i++) {
        const entity texture = zox_get_child_by_id(world, blocks->value[i], zox_id(TextureFrames));
        if (zox_valid(texture) && zox_getv(texture, GenerateTexture)) {
            return 0;
        }
    }
    return 1;
}

// how different two colours are, summed over rgb
static inline int zox_tst_bframes_diff(color a, color b) {
    return abs(a.r - b.r) + abs(a.g - b.g) + abs(a.b - b.b);
}

// two frames' difference, every pixel
static inline long zox_tst_bframes_frames_diff(const color* a, const color* b, int length) {
    long sum = 0;
    for (int i = 0; i < length; i++) {
        sum += zox_tst_bframes_diff(a[i], b[i]);
    }
    return sum;
}

static inline void zox_tst_bframes_blit(byte* image, int2 image_size, int2 at, const color* pixels, int2 size) {
    for (int y = 0; y < size.y * zox_tst_bframes_scale; y++) for (int x = 0; x < size.x * zox_tst_bframes_scale; x++) {
        const color c = pixels[(y / zox_tst_bframes_scale) * size.x + x / zox_tst_bframes_scale];
        byte* p = &image[((at.y + y) * image_size.x + at.x + x) * 3];
        p[0] = c.r;
        p[1] = c.g;
        p[2] = c.b;
    }
}

void zox_tst_block_frames_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const BlockLinks* blocks = zox_get(zox_get_link(world, character, RealmLink), BlockLinks);
    int animated = 0;
    mkdir(zox_tst_path("blocks"), 0755);
    for (int i = 0; i < blocks->length; i++) {
        const entity block = blocks->value[i];
        const entity texture = zox_valid(block) ? zox_get_child_by_id(world, block, zox_id(TextureFrames)) : 0;
        if (!zox_valid(texture)) {
            continue;
        }
        const int2 size = zox_getv(texture, TextureSize);
        const int length = size.x * size.y;
        const TextureFrames* frames = zox_get(texture, TextureFrames);
        const int count = length ? frames->length / length : 0;
        if (count < 2) {
            continue;
        }
        animated++;
        const char* name = zox_get_name(block);
        // each frame against the next, and the last against the first
        long steps = 0, most = 0, least = -1;
        for (int f = 0; f + 1 < count; f++) {
            const long d = zox_tst_bframes_frames_diff(&frames->value[f * length], &frames->value[(f + 1) * length], length);
            steps += d;
            most = d > most ? d : most;
            least = least < 0 || d < least ? d : least;
        }
        const long loop = zox_tst_bframes_frames_diff(&frames->value[(count - 1) * length], frames->value, length);
        // the first frame's columns and rows: each against the next, the last against the first (its seams)
        long columns = 0, column_seam = 0, rows = 0, row_seam = 0;
        for (int y = 0; y < size.y; y++) for (int x = 0; x < size.x; x++) {
            const color c = frames->value[y * size.x + x];
            const long right = zox_tst_bframes_diff(c, frames->value[y * size.x + (x + 1) % size.x]);
            const long down = zox_tst_bframes_diff(c, frames->value[((y + 1) % size.y) * size.x + x]);
            if (x + 1 < size.x) columns += right; else column_seam += right;
            if (y + 1 < size.y) rows += down; else row_seam += down;
        }
        columns /= size.x - 1;
        rows /= size.y - 1;
        zox_log("[tst] [metric] block_frames [%s] [%i] frames of [%ix%i]: a step [%li] to [%li], the loop's join [%li]; a column step [%li], its seam [%li]; a row step [%li], its seam [%li]",
            name, count, size.x, size.y, least, most, loop, columns, column_seam, rows, row_seam);
        zox_tst_check(least > 0, "[%s] has two frames the same", name);
        zox_tst_check(loop <= most * 2, "[%s]'s last frame jumps to its first ([%li], a step is at most [%li])", name, loop, most);
        zox_tst_check(column_seam <= columns * 3 && row_seam <= rows * 3, "[%s] doesn't tile: its seams [%li] [%li] against its steps [%li] [%li]",
            name, column_seam, row_seam, columns, rows);
        // the image: its frames in a row, under them its first tiled 2 x 2 and its last then its first
        const int2 tile = { size.x * zox_tst_bframes_scale, size.y * zox_tst_bframes_scale };
        const int gap = 4;
        const int2 image_size = { int_max(count * (tile.x + gap), 4 * (tile.x + gap)), tile.y * 3 + gap * 2 };
        byte* image = calloc((size_t) image_size.x * image_size.y, 3);
        for (int f = 0; f < count; f++) {
            zox_tst_bframes_blit(image, image_size, (int2) { f * (tile.x + gap), 0 }, &frames->value[f * length], size);
        }
        for (int t = 0; t < 4; t++) {
            zox_tst_bframes_blit(image, image_size, (int2) { (t % 2) * tile.x, tile.y + gap + (t / 2) * tile.y }, frames->value, size);
        }
        zox_tst_bframes_blit(image, image_size, (int2) { 3 * (tile.x + gap), tile.y + gap }, &frames->value[(count - 1) * length], size);
        zox_tst_bframes_blit(image, image_size, (int2) { 3 * (tile.x + gap) + tile.x, tile.y + gap }, frames->value, size);
        // its name without the _<id> on the end
        char base[64];
        snprintf(base, sizeof(base), "%s", name);
        char* underscore = strrchr(base, '_');
        if (underscore && underscore[1] >= '0' && underscore[1] <= '9') {
            *underscore = 0;
        }
        char file[128];
        snprintf(file, sizeof(file), "blocks/%s_frames.ppm", base);
        zox_tst_check(zox_tst_save_ppm(zox_tst_path(file), image, image_size), "couldn't save [%s]", file);
        zox_log("[tst] saved [%s]", zox_tst_path(file));
        free(image);
    }
    zox_tst_check(animated, "the realm has no animated blocks");
}
