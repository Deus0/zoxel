// Gifs: records the screen as an animated gif at some frames a second, i.e. tests looking at an animation
//  - zox_gif_record(path, fps) starts from the next frame drawn, zox_gif_stop() finishes the file
//  - zox_gif_capture(size) takes a frame when one is due: app_gpu_end_system calls it before the swap
//  - each frame has its own 256 colours: the most used of 5 bits a channel, each their pixels' average, the rest
//    their nearest (a voxel scene's few flat colours stay exact)
//  - a frame is written when the next is taken, held for the real time between them: a slow frame rate (software gl)
//    plays at the game's speed, not sped up
#define zox_gif_bins 32768      // colours, 5 bits a channel
#define zox_gif_codes 4096      // lzw's most codes (12 bits)

typedef struct {
    FILE* file;
    int2 size;
    double interval;    // seconds between frames
    double last;        // when the held frame was taken
    byte* held;         // the last frame taken, rgb top row first, written when its time is known
    int frames;
} zox_gif;
zox_gif zox_gif_recorder;
// frames only: each frame drawn kept as the held one (zox_gif_recorder.held), no file, nothing encoded (a test reading
//  every frame: encoding one took ~100ms, its frame rate fell to 10 a second, flickers between frames unseen)
byte zox_gif_frames_only = 0;

// lzw codes packed into bytes, written in blocks of up to 255
typedef struct {
    FILE* file;
    byte block[255];
    int length;
    uint bits;
    int count;
} zox_gif_packer;

static inline void zox_gif_u16(FILE* file, int value) {
    fputc(value & 255, file);
    fputc((value >> 8) & 255, file);
}

static inline void zox_gif_block(zox_gif_packer* packer) {
    if (packer->length) {
        fputc(packer->length, packer->file);
        fwrite(packer->block, 1, packer->length, packer->file);
        packer->length = 0;
    }
}

static inline void zox_gif_code(zox_gif_packer* packer, uint code, int size) {
    packer->bits |= code << packer->count;
    packer->count += size;
    while (packer->count >= 8) {
        packer->block[packer->length++] = packer->bits & 255;
        packer->bits >>= 8;
        packer->count -= 8;
        if (packer->length == 255) {
            zox_gif_block(packer);
        }
    }
}

// the indices lzw compressed, 8 bits a colour: a code for each run seen before, a new code for it and one more
static void zox_gif_lzw(FILE* file, const byte* indices, int length) {
    const int min_size = 8;
    const uint clear = 1 << min_size;
    ushort (*tree)[256] = calloc(zox_gif_codes, sizeof(*tree));  // a code and the next index: its longer code
    zox_gif_packer packer = { .file = file };
    int size = min_size + 1;
    uint last_code = clear + 1;     // the end code, then the codes made
    fputc(min_size, file);
    zox_gif_code(&packer, clear, size);
    uint code = indices[0];
    for (int i = 1; i < length; i++) {
        const byte next = indices[i];
        if (tree[code][next]) {
            code = tree[code][next];
            continue;
        }
        zox_gif_code(&packer, code, size);
        tree[code][next] = ++last_code;
        if (last_code >= (1u << size)) {
            size++;
        }
        if (last_code == zox_gif_codes - 1) {
            // full: start over
            zox_gif_code(&packer, clear, size);
            memset(tree, 0, zox_gif_codes * sizeof(*tree));
            size = min_size + 1;
            last_code = clear + 1;
        }
        code = next;
    }
    zox_gif_code(&packer, code, size);
    zox_gif_code(&packer, clear, size);
    zox_gif_code(&packer, clear + 1, min_size + 1);
    if (packer.count) {
        packer.block[packer.length++] = packer.bits & 255;
        if (packer.length == 255) {
            zox_gif_block(&packer);
        }
    }
    zox_gif_block(&packer);
    fputc(0, file);
    free(tree);
}

static inline int zox_gif_bin(const byte* rgb) {
    return (rgb[0] >> 3) << 10 | (rgb[1] >> 3) << 5 | rgb[2] >> 3;
}

typedef struct {
    uint count;
    ushort bin;
} zox_gif_used;

static int zox_gif_used_compare(const void* a, const void* b) {
    const uint x = ((const zox_gif_used*) a)->count, y = ((const zox_gif_used*) b)->count;
    return x < y ? 1 : x > y ? -1 : 0;
}

// a frame, its own colours, shown for delay (hundredths of a second)
static void zox_gif_frame(FILE* file, const byte* rgb, int2 size, int delay) {
    const int length = size.x * size.y;
    uint* counts = calloc(zox_gif_bins, sizeof(uint));
    uint (*sums)[3] = calloc(zox_gif_bins, sizeof(*sums));
    zox_gif_used* used = malloc(zox_gif_bins * sizeof(zox_gif_used));
    short* nearest = malloc(zox_gif_bins * sizeof(short));
    byte* indices = malloc(length);
    for (int i = 0; i < length; i++) {
        const byte* pixel = &rgb[i * 3];
        const int bin = zox_gif_bin(pixel);
        counts[bin]++;
        sums[bin][0] += pixel[0];
        sums[bin][1] += pixel[1];
        sums[bin][2] += pixel[2];
    }
    // the most used bins are the palette, each its pixels' average
    int used_length = 0;
    for (int bin = 0; bin < zox_gif_bins; bin++) {
        nearest[bin] = -1;
        if (counts[bin]) {
            used[used_length++] = (zox_gif_used) { counts[bin], bin };
        }
    }
    qsort(used, used_length, sizeof(zox_gif_used), zox_gif_used_compare);
    const int colors = used_length < 256 ? used_length : 256;
    byte palette[256][3] = { };
    for (int k = 0; k < used_length; k++) {
        const int bin = used[k].bin;
        const int c[3] = { sums[bin][0] / counts[bin], sums[bin][1] / counts[bin], sums[bin][2] / counts[bin] };
        if (k < colors) {
            palette[k][0] = c[0];
            palette[k][1] = c[1];
            palette[k][2] = c[2];
            nearest[bin] = k;
            continue;
        }
        int best = 0, best_distance = 1 << 30;
        for (int j = 0; j < colors; j++) {
            const int r = c[0] - palette[j][0], g = c[1] - palette[j][1], b = c[2] - palette[j][2];
            const int distance = r * r + g * g + b * b;
            if (distance < best_distance) {
                best_distance = distance;
                best = j;
            }
        }
        nearest[bin] = best;
    }
    for (int i = 0; i < length; i++) {
        indices[i] = nearest[zox_gif_bin(&rgb[i * 3])];
    }
    // its time (left as drawn, the next over it), then the image with its 256 colours
    const byte timing[] = { 0x21, 0xF9, 4, 1 << 2 };
    fwrite(timing, 1, sizeof(timing), file);
    zox_gif_u16(file, delay);
    fputc(0, file);
    fputc(0, file);
    fputc(0x2C, file);
    zox_gif_u16(file, 0);
    zox_gif_u16(file, 0);
    zox_gif_u16(file, size.x);
    zox_gif_u16(file, size.y);
    fputc(0x80 | 7, file);
    fwrite(palette, 1, sizeof(palette), file);
    zox_gif_lzw(file, indices, length);
    free(counts);
    free(sums);
    free(used);
    free(nearest);
    free(indices);
}

static inline void zox_gif_write_held(zox_gif* gif, double seconds) {
    const int delay = (int) (seconds * 100 + 0.5);
    zox_gif_frame(gif->file, gif->held, gif->size, delay < 2 ? 2 : delay);   // under 2 players show slowly
    free(gif->held);
    gif->held = NULL;
    gif->frames++;
}

// finishes the gif being recorded, returns its frames
int zox_gif_stop() {
    zox_gif* gif = &zox_gif_recorder;
    if (!gif->file) {
        return 0;
    }
    if (gif->held) {
        zox_gif_write_held(gif, gif->interval);
    }
    fputc(0x3B, gif->file);
    fclose(gif->file);
    const int frames = gif->frames;
    zox_log("+ gif finished, [%i] frames", frames);
    *gif = (zox_gif) { };
    return frames;
}

// records the screen to a gif at path, fps frames a second, from the next frame drawn; 1 if the file opened
byte zox_gif_record(const char* path, float fps) {
    zox_gif_stop();
    FILE* file = fopen(path, "wb");
    if (!file) {
        zox_loge("failed to open gif [%s]", path);
        return 0;
    }
    zox_gif_recorder = (zox_gif) { .file = file, .interval = 1.0 / (fps > 0 ? fps : 10) };
    return 1;
}

static inline byte zox_gif_recording() {
    return zox_gif_recorder.file != NULL;
}

// takes a frame if one is due, before the buffer swap; size: the window's
void zox_gif_capture(int2 size) {
    zox_gif* gif = &zox_gif_recorder;
    if (zox_gif_frames_only) {
        byte* pixels = zox_gpu_read_screen(size);
        if (pixels) {
            free(gif->held);
            gif->held = pixels;
            gif->size = size;
            gif->last = zox_current_time;
            gif->frames++;
        }
        return;
    }
    const byte started = gif->frames || gif->held;
    if (!gif->file || (started && zox_current_time - gif->last < gif->interval)) {
        return;
    }
    if (started && (size.x != gif->size.x || size.y != gif->size.y)) {
        return;     // resized, it keeps its first size
    }
    byte* pixels = zox_gpu_read_screen(size);
    if (!pixels) {
        return;
    }
    if (!started) {
        // its size from the first frame, looping forever
        gif->size = size;
        fwrite("GIF89a", 1, 6, gif->file);
        zox_gif_u16(gif->file, size.x);
        zox_gif_u16(gif->file, size.y);
        const byte screen[] = { 0x70, 0, 0, 0x21, 0xFF, 11, 'N', 'E', 'T', 'S', 'C', 'A', 'P', 'E', '2', '.', '0', 3, 1, 0, 0, 0 };
        fwrite(screen, 1, sizeof(screen), gif->file);
    }
    if (gif->held) {
        zox_gif_write_held(gif, zox_current_time - gif->last);
    }
    gif->held = pixels;
    gif->last = zox_current_time;
}
