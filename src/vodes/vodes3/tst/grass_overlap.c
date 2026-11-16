// tst: grass_overlap, no two visible vox instances (grass, flowers) at one place: overlapping copies were seen,
//  one at another lod over the other
//  - the player spawned, streaming settled: every frame for 10s, each visible RendererInstance with an InstanceLink
//      at a place (Position3D) another visible one also is, is a copy
//  - also walks the player 24 blocks (lods change round it) and checks again
//  - logs a copy's two: their names, chunks, chunk depths, render depths and lod meshes
//  - run: bash bsh/tst.sh grass_overlap (and with zox_tst_settings=<your settings.cfg> for Block Res 5)
typedef struct {
    int3 at;
    entity e;
} zox_tst_overlap_spot;

#define zox_tst_overlap_max 65536
zox_tst_overlap_spot zox_tst_overlap_spots[zox_tst_overlap_max];
int zox_tst_overlap_count = 0;
int zox_tst_overlap_copies = 0;     // most copies seen in a frame
int zox_tst_overlap_frames = 0;     // frames sampled
int zox_tst_overlap_most = 0;       // most instances seen in a frame
byte zox_tst_overlap_logged = 0;
byte zox_tst_overlap_sampling = 0;
byte zox_tst_overlap_stage = 0;
double zox_tst_overlap_time = 0;

static int zox_tst_overlap_compare(const void* a, const void* b) {
    const int3 p = ((const zox_tst_overlap_spot*) a)->at, q = ((const zox_tst_overlap_spot*) b)->at;
    return p.x != q.x ? (p.x < q.x ? -1 : 1) : p.y != q.y ? (p.y < q.y ? -1 : 1) : p.z != q.z ? (p.z < q.z ? -1 : 1) : 0;
}

static inline void zox_tst_overlap_log(ecs* world, entity e) {
    zox_log("[tst]   [%s] %lu parent [%s] %lu render depth [%i] disabled [%i] mesh [%s] %lu",
        zox_getn(e), e, zox_getn(zox_get_parent(world, e)), zox_get_parent(world, e), zox_getv(e, RenderDepth), zox_getv(e, RenderDisabled), zox_getn(zox_get_link(world, e, InstanceLink)),
        zox_get_link(world, e, InstanceLink));
}

void tst_grass_overlap_system(iter* it) {
    zox_sys_on_begin();
    if (!zox_tst_overlap_sampling) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    zox_sys_in(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(Position3D, position);
        if (disabled->value || !zox_valid(zox_get_link(world, e, InstanceLink)) || zox_tst_overlap_count >= zox_tst_overlap_max) {
            continue;
        }
        const float3 p = position->value;
        zox_tst_overlap_spots[zox_tst_overlap_count++] = (zox_tst_overlap_spot) {
            .at = { (int) floorf(p.x * 64), (int) floorf(p.y * 64), (int) floorf(p.z * 64) }, .e = e };
    }
    zox_sys_on_end();
} zoxd_system(tst_grass_overlap_system);

// after the probe ran this frame: the copies among the places gathered
static inline void zox_tst_overlap_count_copies(ecs* world) {
    if (!zox_tst_overlap_count) {
        return;
    }
    qsort(zox_tst_overlap_spots, zox_tst_overlap_count, sizeof(zox_tst_overlap_spot), zox_tst_overlap_compare);
    int copies = 0;
    for (int i = 1; i < zox_tst_overlap_count; i++) {
        if (zox_tst_overlap_compare(&zox_tst_overlap_spots[i - 1], &zox_tst_overlap_spots[i])) {
            continue;
        }
        copies++;
        if (zox_tst_overlap_logged < 4) {
            zox_tst_overlap_logged++;
            const int3 a = zox_tst_overlap_spots[i].at;
            zox_log("[tst] grass_overlap: a copy at [%.2f %.2f %.2f]", a.x / 64.0f, a.y / 64.0f, a.z / 64.0f);
            zox_tst_overlap_log(world, zox_tst_overlap_spots[i - 1].e);
            zox_tst_overlap_log(world, zox_tst_overlap_spots[i].e);
        }
    }
    zox_tst_overlap_copies = copies > zox_tst_overlap_copies ? copies : zox_tst_overlap_copies;
    zox_tst_overlap_most = zox_tst_overlap_count > zox_tst_overlap_most ? zox_tst_overlap_count : zox_tst_overlap_most;
    zox_tst_overlap_frames++;
    zox_tst_overlap_count = 0;
}

byte zox_tst_grass_overlap_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_overlap_count_copies(world);
    zox_tst_progress(zox_tst_overlap_stage * 1000 + zox_tst_overlap_frames);
    if (!zox_tst_overlap_time) {
        zox_tst_overlap_time = zox_current_time;
        zox_tst_overlap_sampling = 1;
    }
    const double since = zox_current_time - zox_tst_overlap_time;
    if (zox_tst_overlap_stage == 0) {
        zox_tst_waiting = "10s of frames, standing";
        if (since < 10) {
            return 0;
        }
        // walk away 24 blocks: lods change round the player
        const entity character = zox_tst_player_character(world);
        const entity terrain = zox_get_link(world, character, TerrainLink);
        const float scale = zox_getv(terrain, BlockScale);
        float3 p = zox_getv(character, Position3D);
        p.x += 24 * scale;
        p.y += 4 * scale;
        zox_setv(character, Position3D, p);
        zox_setv(character, Velocity3D, float3_zero);
        zox_tst_overlap_stage = 1;
        zox_tst_overlap_time = zox_current_time;
        return 0;
    }
    zox_tst_waiting = "15s of frames, moved";
    return since >= 15;
}

void zox_tst_grass_overlap_check(ecs* world) {
    (void) world;
    zox_tst_overlap_sampling = 0;
    zox_log("[tst] [metric] grass_overlap [%i] frames, up to [%i] visible instances, up to [%i] copies in a frame",
        zox_tst_overlap_frames, zox_tst_overlap_most, zox_tst_overlap_copies);
    zox_tst_check(zox_tst_overlap_most, "no visible vox instances seen");
    zox_tst_check(!zox_tst_overlap_copies, "[%i] vox instances drawn over another at the same place", zox_tst_overlap_copies);
}

void zox_tsts_grass_overlap(ecs* world) {
    zox_tst_add("grass_overlap", zox_tst_grass_overlap_ready, zox_tst_grass_overlap_check, 90);
    zox_system_1(
        tst_grass_overlap_system,
        zoxp_render,
        [in] rendering.RenderDisabled,
        [in] transforms3.Position3D,
        [none] rendering.RendererInstance
    );
}
