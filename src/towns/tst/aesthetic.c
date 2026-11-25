// tst: aesthetic_town, the readme's screenshots from a town's wall, to refresh them as the game grows
//  - the town nearest the player (any region), the wall across from its gate, the camera freed (as free roam does) onto it:
//      wall: along the wall top, the player standing on it, the town to one side, the wild to the other
//      inside: from the wall's inner edge, over the town and on to the far wall
//      outside: from its outer edge, out over the land
//  - each to tst/town/aesthetic/<shot>.ppm; with --readme bsh/tst.sh converts them to doc/screenshot/<shot>.jpg
//  - run: bash bsh/tools/view_town.sh --readme (far, tall terrain, 1080p, little fog, no hud or labels, seed 375758414)
#define zox_tst_aesthetic_shots 3
const char* zox_tst_aesthetic_names[zox_tst_aesthetic_shots] = { "wall", "inside", "outside" };
byte zox_tst_aesthetic_stage = 0;
double zox_tst_aesthetic_time = 0;
entity zox_tst_aesthetic_town = 0;

// the highest solid voxel in a column (unloaded chunks skipped), -1 if none
static inline int zox_tst_aesthetic_top(ecs* world, entity terrain, int2 column) {
    for (int y = 255; y >= 0; y--) {
        if (terrain_voxel_at(world, terrain, (int3) { column.x, y, column.y }) > 0) {
            return y;
        }
    }
    return -1;
}

// a column's top as a position, a block over it (its middle), -1 tops held high until they load
static inline float3 zox_tst_aesthetic_at(ecs* world, entity terrain, int2 column, float above, float scale) {
    int top = zox_tst_aesthetic_top(world, terrain, column);
    if (top < 0) {
        top = 120;
        zox_tst_aesthetic_time = zox_current_time;
    }
    return (float3) { (column.x + 0.5f) * scale, (top + 1 + above) * scale, (column.y + 0.5f) * scale };
}

static inline int2 zox_tst_aesthetic_step(int2 from, int2 direction, int steps) {
    return (int2) { from.x + direction.x * steps, from.y + direction.y * steps };
}

// the town nearest the player, in any region (a big world's player may land before towns spawn, not at a home)
static inline entity zox_tst_aesthetic_find_town(ecs* world, entity character, float scale) {
    const float3 p = zox_getv(character, Position3D);
    const int2 player = { p.x / scale, p.z / scale };
    entity nearest = 0;
    ecs_iter_t it = ecs_each_id(world, zox_id(Town));
    while (ecs_each_next(&it)) {
        for (int i = 0; i < it.count; i++) {
            const entity town = it.entities[i];
            if (zox_has(town, BlockPosition2) && (!nearest || int2_distance(zox_getv(town, BlockPosition2), player) < int2_distance(zox_getv(nearest, BlockPosition2), player))) {
                nearest = town;
            }
        }
    }
    return nearest;
}

// the player spawned and a town found (zox_tst_aesthetic_town), stage 1 after: 0 waiting, 1 found, 2 none
//  - a big world spawns its towns after the player lands: waits for one, a minute at most
static inline byte zox_tst_aesthetic_wait_town(ecs* world, entity character) {
    if (zox_tst_aesthetic_stage) {
        return 1;
    }
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    if (!zox_tst_aesthetic_time) {
        zox_tst_aesthetic_time = zox_current_time;
    }
    zox_tst_waiting = "a town";
    zox_tst_progress((int) (zox_current_time - zox_tst_aesthetic_time));
    zox_tst_aesthetic_town = zox_tst_aesthetic_find_town(world, character, zox_getv(zox_get_link(world, character, TerrainLink), BlockScale));
    if (!zox_valid(zox_tst_aesthetic_town)) {
        if (zox_current_time - zox_tst_aesthetic_time < 60) {
            return 0;
        }
        zox_tst_check(0, "no town");
        return 2;
    }
    zox_tst_aesthetic_stage = 1;
    zox_tst_aesthetic_time = zox_current_time;
    return 1;
}

// the wall across from the town's gate: its middle, out of the town and along it
static inline void zox_tst_aesthetic_wall(ecs* world, int2* middle, int2* out, int2* along, int2* inner, int2* outer) {
    const entity town = zox_tst_aesthetic_town;
    const int2 center = zox_getv(town, BlockPosition2);
    const byte2 size = zox_getv(town, TownSize);
    const int thick = zox_getv(town, WallThickness);
    // the wall across from the gate (sides: 0 +z, 1 +x, 2 -z, 3 -x), out its normal, along it its tangent
    const byte side = (zox_getv(town, TownGateSide) + 2) % 4;
    const int2 normals[4] = { { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };
    *out = normals[side];
    *along = (int2) { -out->y, out->x };
    const int half = out->x ? size.x / 2 : size.y / 2;
    *middle = zox_tst_aesthetic_step(center, *out, half - thick / 2);
    *inner = zox_tst_aesthetic_step(center, *out, half - thick + 1);
    *outer = zox_tst_aesthetic_step(center, *out, half);
}

// the camera freed from the player (as free roam does), at eye looking at target
static inline entity zox_tst_aesthetic_camera(ecs* world, float3 eye, float3 target) {
    const entity camera = zox_get_link(world, dbg_player, CameraLink);
    if (!zox_valid(camera)) {
        return 0;
    }
    if (zox_get_parent(world, camera)) {
        zox_set_parent(world, camera, 0);
    }
    zox_setv(camera, Position3D, eye);
    zox_setv(camera, Rotation3D, camera_look_at(eye, target));
    return camera;
}

byte zox_tst_aesthetic_town_ready(ecs* world) {
    const entity character = zox_tst_player_character(world);
    const byte town_found = zox_tst_aesthetic_wait_town(world, character);
    if (town_found != 1) {
        return town_found == 2;
    }
    zox_tst_waiting = "the aesthetic shots";
    zox_tst_progress(zox_tst_aesthetic_stage * 100 + (int) (zox_current_time - zox_tst_aesthetic_time));
    if (zox_screenshot_request[0]) {
        return 0;
    }
    const byte shot = zox_tst_aesthetic_stage - 1;
    if (shot >= zox_tst_aesthetic_shots) {
        return 1;
    }
    const entity terrain = zox_get_link(world, character, TerrainLink);
    const float scale = zox_getv(terrain, BlockScale);
    int2 middle, out, along, inner, outer;
    zox_tst_aesthetic_wall(world, &middle, &out, &along, &inner, &outer);
    // the player on the wall top, back to the camera, looking along it
    const float3 player = zox_tst_aesthetic_at(world, terrain, zox_tst_aesthetic_step(middle, along, -15), 0, scale);
    zox_setv(character, Position3D, player);
    zox_setv(character, Velocity3D, float3_zero);
    zox_setv(character, RenderDisabled, 0);
    zox_setv(character, Rotation3D, camera_look_at(float3_zero, (float3) { -along.x, 0, -along.y }));
    float3 eye, target;
    if (shot == 0) {
        eye = zox_tst_aesthetic_at(world, terrain, zox_tst_aesthetic_step(middle, along, -20), 5, scale);
        target = zox_tst_aesthetic_at(world, terrain, zox_tst_aesthetic_step(middle, along, 24), -4, scale);
    } else {
        const int2 from = shot == 1 ? inner : outer, facing = shot == 1 ? (int2) { -out.x, -out.y } : out;
        eye = zox_tst_aesthetic_at(world, terrain, zox_tst_aesthetic_step(from, along, 6), 6, scale);
        target = float3_add(eye, (float3) { facing.x * 40 * scale, -6 * scale, facing.y * 40 * scale });
    }
    zox_tst_aesthetic_camera(world, eye, target);
    // the first waits for the far land, the others for the camera to settle
    if (zox_current_time - zox_tst_aesthetic_time < (shot ? 3 : 30)) {
        return 0;
    }
    char name[64];
    snprintf(name, sizeof(name), "aesthetic/%s.ppm", zox_tst_aesthetic_names[shot]);
    snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path(name));
    zox_tst_aesthetic_stage++;
    zox_tst_aesthetic_time = zox_current_time;
    return 0;
}

void zox_tst_aesthetic_town_check(ecs* world) {
    (void) world;
    for (int i = 0; i < zox_tst_aesthetic_shots; i++) {
        char name[64];
        snprintf(name, sizeof(name), "aesthetic/%s.ppm", zox_tst_aesthetic_names[i]);
        zox_tst_check(zox_tst_clouds_share(zox_tst_path(name)) >= 0, "no [%s] screenshot saved", zox_tst_aesthetic_names[i]);
    }
}

// tst: aesthetic_particles, the readme's particles gif: bursts of colour rising out of the player, see
//  through, fading (particles3 sys/fade.c), the camera freed in front of the player looking back at them
//  - after the spawn the land loads a while, then particles.gif (3s at 12 fps) in the test output; with --readme
//    bsh/tst.sh shrinks it to doc/gifs/particles.gif
//  - close: a particle is a vox voxel, a few pixels at more than a couple of metres
//  - run: bash bsh/tools/view_particles.sh --readme (1080p, no hud, the bright seed)
#define zox_tst_aesthetic_particles_seconds 3.0
#define zox_tst_aesthetic_particles_every 0.5     // seconds between bursts
int zox_tst_aesthetic_particles_frames = 0;
int zox_tst_aesthetic_particles_bursts = 0;
static int zox_tst_aesthetic_particles_settled_frames = 0;
const color zox_tst_aesthetic_particles_colors[4] = { { 255, 200, 60, 255 }, { 255, 80, 200, 255 }, { 80, 220, 255, 255 },
    { 140, 255, 120, 255 } };

byte zox_tst_aesthetic_particles_ready(ecs* world) {
    if (!zox_tst_aesthetic_stage) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_aesthetic_stage = 1;
        zox_tst_aesthetic_time = zox_current_time;
    }
    zox_tst_waiting = "the aesthetic particles";
    zox_tst_progress(zox_tst_aesthetic_stage * 100 + (int) (zox_current_time - zox_tst_aesthetic_time));
    // the camera freed, out in front of the player a little above, looking back at its chest
    const entity character = zox_tst_player_character(world);
    if (!zox_valid(character)) {
        // the player died (an npc's hit) while it filmed: the gif so far, a failed check
        zox_tst_aesthetic_particles_frames = zox_gif_stop();
        zox_tst_check(0, "the player's character is gone");
        return 1;
    }
    const float3 body = zox_getv(character, Position3D);
    const float4 facing = zox_getv(character, Rotation3D);
    const float3 forward = float3_normalize(float4_rotate_float3(facing, (float3) { 0, 0, 1 }));
    const float3 side = float3_normalize((float3) { -forward.z, 0, forward.x });
    const float3 chest = float3_add(body, (float3) { 0, 0.25f, 0 });
    const float3 eye = float3_add(chest, float3_add(float3_scale(forward, 1.5f), float3_add(float3_scale(side, 0.6f), (float3) { 0, 2.0f, 0 })));
    const entity camera = zox_tst_aesthetic_camera(world, eye, chest);
    if (zox_tst_aesthetic_stage == 1) {
        zox_tst_waiting = "the particle capture terrain to settle";
        // Reframing changes the camera's stream point and frustum on subsequent frames.
        if (!camera || zox_getv(camera, StreamDirty) || zox_getv(camera, StreamDirty2) ||
            zox_tst_terrain_busy(world, local_terrain)) {
            zox_tst_aesthetic_particles_settled_frames = 0;
            return 0;
        }
        if (++zox_tst_aesthetic_particles_settled_frames < 5) {
            return 0;
        }
        zox_gif_record(zox_tst_path("./particles.gif"), 12);
        zox_tst_aesthetic_stage = 2;
        zox_tst_aesthetic_time = zox_current_time;
    }
    const double t = zox_current_time - zox_tst_aesthetic_time;
    if (t >= zox_tst_aesthetic_particles_seconds) {
        zox_tst_aesthetic_particles_frames = zox_gif_stop();
        return 1;
    }
    // a burst each half second, a colour each, up out of the player's shoulders and falling back
    if (t >= zox_tst_aesthetic_particles_bursts * zox_tst_aesthetic_particles_every) {
        const float offset = ((zox_tst_aesthetic_particles_bursts % 3) - 1) * 0.25f;
        const particle3_burst burst = {
            .position = float3_add(chest, float3_add(float3_scale(side, offset), (float3) { 0, 0.2f, 0 })),
            .velocity = (float3) { 0, 0.9f, 0 },
            .spread = (float3) { 0.45f, 0.35f, 0.45f },
            .gravity = (float3) { 0, -1.2f, 0 },
            .life = 1.8f,
            .fallback = zox_tst_aesthetic_particles_colors[zox_tst_aesthetic_particles_bursts % 4],
            .count = 240,
        };
        spawn_particle3_burst(world, 0, &burst, 1000 + zox_tst_aesthetic_particles_bursts * 997);
        zox_tst_aesthetic_particles_bursts++;
    }
    return 0;
}

void zox_tst_aesthetic_particles_check(ecs* world) {
    (void) world;
    zox_gif_stop();
    zox_log("[tst] [metric] aesthetic_particles: [%i] gif frames, [%i] bursts", zox_tst_aesthetic_particles_frames,
        zox_tst_aesthetic_particles_bursts);
    zox_tst_check(zox_tst_aesthetic_particles_frames >= 10, "the gif has [%i] frames", zox_tst_aesthetic_particles_frames);
}
