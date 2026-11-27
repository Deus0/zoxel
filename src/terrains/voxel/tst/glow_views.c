// tst: glow_views, a glow stone in a dark brick room through the game's camera and its post pass, four views
//  (bsh/tools/view_glow.sh saves them as pngs to look at): clamp, bloom, soft tonemap with bloom, filmic with bloom
//  - an all air chunk in the void (air_chunk.c): a closed brick room (inside 4 to 10, floor y 3, roof y 10), no sky
//    in, a glow stone on its floor in the middle, a street lamp beside it (a block vox, its glass a palette glow)
//  - the menu's camera (a render camera: tonemap, bloom) inside by a wall, looking down at it, ui hidden
//  - checks: each view saved; the glow stone brighter than the room's far corner; the lamp's glass past white (its
//    blue clipped, ~200 unglowing); bloom brightens the area round the stone
//  - run: bash bsh/tst.sh glow_views, or bash bsh/tools/view_glow.sh
typedef struct {
    byte tonemap;
    byte bloom;
    const char* file;
} zox_tst_gviews_view;

const zox_tst_gviews_view zox_tst_gviews_views[4] = {
    { 0, 0, "glow_clamp.ppm" },
    { 0, 1, "glow_bloom.ppm" },
    { 1, 1, "glow_soft_bloom.ppm" },
    { 2, 1, "glow_filmic_bloom.ppm" } };
byte zox_tst_gviews_stage = 0;
byte zox_tst_gviews_view_index = 0;
int zox_tst_gviews_frames = 0;

int zox_tst_gviews_settled = 0;

byte zox_tst_glow_views_ready(ecs* world) {
    const entity chunk = zox_tst_streamed_air_chunk(world);
    if (!chunk) {
        return 0;
    }
    if (zox_tst_gviews_stage == 0) {
        const byte bricks = zox_getv(zox_tst_air_block(world, zox_id(BlockBricks)), BlockIndex);
        const byte glow = zox_getv(zox_tst_air_block(world, zox_id(BlockGlowStone)), BlockIndex);
        for (byte x = 3; x <= 11; x++) for (byte y = 3; y <= 10; y++) for (byte z = 3; z <= 11; z++) {
            const byte inside = x >= 4 && x <= 10 && y >= 4 && y <= 9 && z >= 4 && z <= 10;
            if (!inside) {
                zox_tst_room_set(world, chunk, (byte3) { x, y, z }, bricks, 0);
            }
        }
        zox_tst_room_set(world, chunk, (byte3) { 7, 4, 7 }, glow, 0);
        // a street lamp beside it, its base and two fillers (as towns place them): its glass glows (ColorGlows)
        const entity lamp = zox_tst_air_block(world, zox_id(BlockLamp));
        const entity filler = zox_tst_air_block(world, zox_id(BlockFiller));
        zox_tst_check(zox_valid(lamp) && zox_valid(filler), "the realm has no street lamp or filler");
        if (zox_valid(lamp) && zox_valid(filler)) {
            for (byte h = 0; h < 3; h++) {
                zox_tst_room_set(world, chunk, (byte3) { 5, 4 + h, 6 }, zox_getv(h ? filler : lamp, BlockIndex),
                    h ? block_offset_encode((int3) { 0, -h, 0 }) : 1);
            }
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
        // the lamp's model, spawned by the vodes spawner (vodes3/sys/spawn.c) and linked to its base voxel
        zox_tst_waiting = "the street lamp's model to spawn";
        const VoxelNode* base = get_VoxelNode(zox_get(chunk, VoxelNode), zox_getv(chunk, NodeDepth), (byte3) { 5, 4, 6 });
        if (!base || !is_linked_VoxelNode(base)) {
            return 0;
        }
        const float3 target = zox_tst_air_position(world, chunk, (float3) { 7, 4, 7 });
        const float3 eye = zox_tst_air_position(world, chunk, (float3) { 7, 8, 10 });
        zox_tst_lids_make_void(world, camera);
        zox_setv(camera, Color, ((color) { 0, 0, 0, 255 }));
        zox_setv(camera, Position3D, eye);
        zox_setv(camera, Rotation3D, camera_look_at(eye, target));
        nohud = 1;
        zox_tst_gviews_stage = 2;
        return 0;
    }
    zox_tst_waiting = "the glow's views";
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

// the most blue of the image's red pixels (past 200) in its top left (the lamp's glass): its glass, (255, 226, 150)
//  unglowing, past white glowing
static inline int zox_tst_gviews_lamp_blue(const byte* pixels, int2 size) {
    int most = 0;
    for (int x = 0; x < (int) (0.35f * size.x); x++) {
        for (int y = 0; y < (int) (0.45f * size.y); y++) {
            const byte* p = zox_tst_ppm_pixel(pixels, size, x, y);
            if (p[0] > 200 && p[2] > most) {
                most = p[2];
            }
        }
    }
    return most;
}

void zox_tst_glow_views_check(ecs* world) {
    (void) world;
    int middle[4] = { 0 }, beside[4] = { 0 }, corner[4] = { 0 }, lamp[4] = { 0 };
    for (byte i = 0; i < 4; i++) {
        int2 size;
        byte* image = zox_tst_load_ppm(zox_tst_path(zox_tst_gviews_views[i].file), &size);
        zox_tst_check(image, "[%s] wasn't saved", zox_tst_gviews_views[i].file);
        if (!image) {
            return;
        }
        middle[i] = zox_tst_lids_at(image, size, 0.5f, 0.5f);
        beside[i] = zox_tst_gviews_area(image, size, 0.15f);    // round it, its glow
        corner[i] = zox_tst_lids_at(image, size, 0.03f, 0.03f);
        lamp[i] = zox_tst_gviews_lamp_blue(image, size);
        free(image);
    }
    zox_log("[tst] glow_views: the glow stone [%i %i %i %i], round it [%i %i %i %i], a corner [%i %i %i %i]",
        middle[0], middle[1], middle[2], middle[3], beside[0], beside[1], beside[2], beside[3],
        corner[0], corner[1], corner[2], corner[3]);
    zox_tst_check(middle[0] > corner[0] + 100, "the glow stone [%i] isn't brighter than the room's corner [%i]",
        middle[0], corner[0]);
    zox_log("[tst] glow_views: the lamp's glass, blue [%i %i %i %i]", lamp[0], lamp[1], lamp[2], lamp[3]);
    zox_tst_check(lamp[0] > 235, "the lamp's glass doesn't glow: blue [%i] (~200 unglowing, its ColorGlows)", lamp[0]);
    zox_tst_check(beside[1] >= beside[0] + 3, "bloom didn't brighten round the glow stone [%i to %i]", beside[0], beside[1]);
}
