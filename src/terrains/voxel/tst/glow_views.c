// tst: glow_views and lamp_views, glowing things in the dark through the game's camera and its post pass, four
//  views each (bsh/tools/view_glow.sh saves them as pngs to look at): clamp, bloom, soft tonemap with bloom, filmic
//  with bloom; an all air chunk in the void (air_chunk.c), the menu's camera inside, ui hidden
//  - glow_views: a closed brick room (inside 4 to 10, floor y 3, roof y 10), a glow stone (BlockGlow, baked into the
//    chunk's mesh) on its floor in the middle, the camera by a wall looking down at it
//    - checks: the stone brighter than the room's far corner; bloom brightens round it
//  - lamp_views: a rough stone cave (the chunk's width, inside 1 to 14, floor y 3), lit by a street lamp alone (a block vox, its glass a
//    palette glow, ColorGlows), the camera near its head
//    - checks: the glass past white (its blue clipped, ~200 unglowing); bloom brightens round it
//  - checks, both: each view saved
//  - run: bash bsh/tools/view_glow.sh [--lamp], or bash bsh/tst.sh glow_views / lamp_views
typedef struct {
    byte tonemap;
    byte bloom;
    const char* file;
} zox_tst_gviews_view;

const zox_tst_gviews_view zox_tst_gviews_views[4] = {
    { 0, 0, "clamp.ppm" },
    { 0, 1, "bloom.ppm" },
    { 1, 1, "soft_bloom.ppm" },
    { 2, 1, "filmic_bloom.ppm" } };
byte zox_tst_gviews_stage = 0;
byte zox_tst_gviews_view_index = 0;
int zox_tst_gviews_frames = 0;
int zox_tst_gviews_settled = 0;

// glow_views' room: bricks round it, a glow stone in the middle of its floor
static inline void zox_tst_gviews_build_room(ecs* world, entity chunk) {
    const byte bricks = zox_getv(zox_tst_air_block(world, zox_id(BlockBricks)), BlockIndex);
    const byte glow = zox_getv(zox_tst_air_block(world, zox_id(BlockGlowStone)), BlockIndex);
    for (byte x = 3; x <= 11; x++) for (byte y = 3; y <= 10; y++) for (byte z = 3; z <= 11; z++) {
        if (!(x >= 4 && x <= 10 && y >= 4 && y <= 9 && z >= 4 && z <= 10)) {
            zox_tst_room_set(world, chunk, (byte3) { x, y, z }, bricks, 0);
        }
    }
    zox_tst_room_set(world, chunk, (byte3) { 7, 4, 7 }, glow, 0);
}

// lamp_views' cave: stone round it, its walls and roof rough (a block in now and then), a street lamp in the middle
//  of its floor, its base and two fillers as towns place them, the far walls ~10 blocks off
static inline void zox_tst_gviews_build_cave(ecs* world, entity chunk) {
    // stone is a biome's (heights/sys/landfill.c), the first in the realm's blocks
    byte stone = 0;
    const BlockLinks* blocks = zox_get(zox_tst_world_realm, BlockLinks);
    for (int i = 0; i < blocks->length && !stone; i++) {
        if (zox_valid(blocks->value[i]) && zox_has(blocks->value[i], BlockStone)) {
            stone = zox_getv(blocks->value[i], BlockIndex);
        }
    }
    zox_tst_check(stone, "the realm has no stone");
    for (byte x = 0; x <= 15; x++) for (byte y = 3; y <= 15; y++) for (byte z = 0; z <= 15; z++) {
        const byte inside = x >= 1 && x <= 14 && y >= 4 && y <= 14 && z >= 1 && z <= 14;
        const byte edge = x == 1 || x == 14 || z == 1 || z == 14 || y == 14;
        const byte bump = edge && ((x * 7 + y * 13 + z * 5) % 4 == 0);
        if (!inside || bump) {
            zox_tst_room_set(world, chunk, (byte3) { x, y, z }, stone, 0);
        }
    }
    const entity lamp = zox_tst_air_block(world, zox_id(BlockLamp));
    const entity filler = zox_tst_air_block(world, zox_id(BlockFiller));
    zox_tst_check(zox_valid(lamp) && zox_valid(filler), "the realm has no street lamp or filler");
    if (zox_valid(lamp) && zox_valid(filler)) {
        for (byte h = 0; h < 3; h++) {
            zox_tst_room_set(world, chunk, (byte3) { 7, 4 + h, 7 }, zox_getv(h ? filler : lamp, BlockIndex),
                h ? block_offset_encode((int3) { 0, -h, 0 }) : 1);
        }
    }
}

// builds its scene, waits for it to light (and the lamp's model to spawn), then saves each view
byte zox_tst_gviews_ready(ecs* world, byte lamp) {
    const entity chunk = zox_tst_streamed_air_chunk(world);
    if (!chunk) {
        return 0;
    }
    if (zox_tst_gviews_stage == 0) {
        if (lamp) {
            zox_tst_gviews_build_cave(world, chunk);
        } else {
            zox_tst_gviews_build_room(world, chunk);
        }
        zox_tst_gviews_stage = 1;
        return 0;
    }
    const entity camera = main_cameras[0];
    if (zox_tst_gviews_stage == 1) {
        zox_tst_waiting = "the room built and lit";
        if (!zox_tst_room_settled(world, chunk, &zox_tst_gviews_settled)) {
            return 0;
        }
        if (lamp) {
            // the lamp's model, spawned by the vodes spawner (vodes3/sys/spawn.c) and linked to its base voxel
            zox_tst_waiting = "the street lamp's model to spawn";
            const VoxelNode* base = get_VoxelNode(zox_get(chunk, VoxelNode), zox_getv(chunk, NodeDepth), (byte3) { 7, 4, 7 });
            if (!base || !is_linked_VoxelNode(base)) {
                return 0;
            }
        }
        // the stone from above by a wall; the lamp's head from a few blocks off, a little below it
        const float3 target = zox_tst_air_position(world, chunk, lamp ? (float3) { 7, 6.2f, 7 } : (float3) { 7, 4, 7 });
        const float3 eye = zox_tst_air_position(world, chunk, lamp ? (float3) { 9, 5.8f, 9.6f } : (float3) { 7, 8, 10 });
        zox_tst_lids_make_void(world, camera);
        zox_setv(camera, Color, ((color) { 0, 0, 0, 255 }));
        zox_setv(camera, Position3D, eye);
        zox_setv(camera, Rotation3D, camera_look_at(eye, target));
        nohud = 1;
        zox_tst_gviews_stage = 2;
        return 0;
    }
    zox_tst_waiting = "the views";
    zox_tst_progress(zox_tst_gviews_view_index * 1000 + zox_tst_gviews_frames);
    if (zox_screenshot_request[0]) {
        return 0;
    }
    if (zox_tst_gviews_view_index >= 4) {
        return 1;
    }
    const zox_tst_gviews_view* view = &zox_tst_gviews_views[zox_tst_gviews_view_index];
    if (++zox_tst_gviews_frames == 1) {
        zox_set_tonemap(world, view->tonemap);
        zox_set_bloom(world, view->bloom);
        return 0;
    }
    if (zox_tst_gviews_frames < 15) {
        return 0;
    }
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(view->file));
    zox_tst_gviews_view_index++;
    zox_tst_gviews_frames = 0;
    return 0;
}


byte zox_tst_glow_views_ready(ecs* world) {
    return zox_tst_gviews_ready(world, 0);
}

byte zox_tst_lamp_views_ready(ecs* world) {
    return zox_tst_gviews_ready(world, 1);
}

// the mean brightness (0 to 765) of the image's middle, from 0.5 - half to 0.5 + half
static inline int zox_tst_gviews_area(const byte* pixels, int2 size, float half) {
    long sum = 0;
    int count = 0;
    for (int x = (int) ((0.5f - half) * size.x); x < (int) ((0.5f + half) * size.x); x++) {
        for (int y = (int) ((0.5f - half) * size.y); y < (int) ((0.5f + half) * size.y); y++) {
            const byte* p = zox_tst_ppm_pixel(pixels, size, x, y);
            sum += p[0] + p[1] + p[2];
            count++;
        }
    }
    return count ? (int) (sum / count) : 0;
}

// the most blue of the red pixels (past 200) in the image's middle (the lamp's glass): (255, 226, 150) unglowing,
//  past white glowing
static inline int zox_tst_gviews_lamp_blue(const byte* pixels, int2 size) {
    int most = 0;
    for (int x = (int) (0.3f * size.x); x < (int) (0.7f * size.x); x++) {
        for (int y = (int) (0.3f * size.y); y < (int) (0.7f * size.y); y++) {
            const byte* p = zox_tst_ppm_pixel(pixels, size, x, y);
            if (p[0] > 200 && p[2] > most) {
                most = p[2];
            }
        }
    }
    return most;
}

// each view's reads: the middle, round it, a corner, the lamp's glass blue; 0 if any view wasn't saved
static inline byte zox_tst_gviews_reads(int* middle, int* beside, int* corner, int* glass) {
    for (byte i = 0; i < 4; i++) {
        int2 size;
        byte* image = zox_tst_load_ppm(zox_tst_path(zox_tst_gviews_views[i].file), &size);
        zox_tst_check(image, "[%s] wasn't saved", zox_tst_gviews_views[i].file);
        if (!image) {
            return 0;
        }
        middle[i] = zox_tst_lids_at(image, size, 0.5f, 0.5f);
        beside[i] = zox_tst_gviews_area(image, size, 0.09f);    // just round its glow, the halo not diluted by the room
        corner[i] = zox_tst_lids_at(image, size, 0.03f, 0.03f);
        glass[i] = zox_tst_gviews_lamp_blue(image, size);
        free(image);
    }
    return 1;
}

void zox_tst_glow_views_check(ecs* world) {
    (void) world;
    int middle[4] = { 0 }, beside[4] = { 0 }, corner[4] = { 0 }, glass[4] = { 0 };
    if (!zox_tst_gviews_reads(middle, beside, corner, glass)) {
        return;
    }
    zox_log("[tst] glow_views: the glow stone [%i %i %i %i], round it [%i %i %i %i], a corner [%i %i %i %i]",
        middle[0], middle[1], middle[2], middle[3], beside[0], beside[1], beside[2], beside[3],
        corner[0], corner[1], corner[2], corner[3]);
    zox_tst_check(middle[0] > corner[0] + 100, "the glow stone [%i] isn't brighter than the room's corner [%i]",
        middle[0], corner[0]);
    zox_tst_check(beside[1] >= beside[0] + 3, "bloom didn't brighten round the glow stone [%i to %i]", beside[0], beside[1]);
}

void zox_tst_lamp_views_check(ecs* world) {
    (void) world;
    int middle[4] = { 0 }, beside[4] = { 0 }, corner[4] = { 0 }, glass[4] = { 0 };
    if (!zox_tst_gviews_reads(middle, beside, corner, glass)) {
        return;
    }
    zox_log("[tst] lamp_views: the lamp's glass, blue [%i %i %i %i], round it [%i %i %i %i], a corner [%i %i %i %i]",
        glass[0], glass[1], glass[2], glass[3], beside[0], beside[1], beside[2], beside[3],
        corner[0], corner[1], corner[2], corner[3]);
    zox_tst_check(glass[0] > 235, "the lamp's glass doesn't glow: blue [%i] (~200 unglowing, its ColorGlows)", glass[0]);
    zox_tst_check(beside[1] >= beside[0] + 3, "bloom didn't brighten round the lamp [%i to %i]", beside[0], beside[1]);
}
