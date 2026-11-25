// tst: stream_seams, every border between chunks is watertight as drawn, where lods meet too (seams.c)
//  - a world of its own, a streamer: checked once it first settles (a game loaded, looking at the distance), then at
//    each step as it walks out and back, a chunk at a time and in jumps, so chunks change lod both ways
//  - each check: every border between two ready chunks, each side's faces read from its meshes (cpu arrays, gpu
//    buffers read back) against what the other side draws (seams.c): no code shared with the meshers, so a wrong rule
//    in them fails here (it replaced stream_sides and stream_gpu_sides, which used the meshers' own rules)
//  - gpu or cpu meshing as the build has it (vulkan: gpu), --gpu_meshing / --cpu_meshing for the other
//  - run: bash bsh/tst.sh stream_seams --vulkan (medium terrain, tst.sh; zox_tst_seed=666 for the same land)
static const int3 zox_tst_seams_steps[] = {
    { 1, 0, 0 }, { 1, 0, 0 }, { 1, 0, 0 }, { 0, 0, 1 }, { 0, 0, 1 },
    { -3, 0, 0 },   // a jump: chunks change lod by more than one
    { 0, 0, -2 }, { 0, 1, 0 }, { 4, 0, 4 }, { -4, -1, -4 } };
#define zox_tst_seams_steps_count ((int) (sizeof(zox_tst_seams_steps) / sizeof(zox_tst_seams_steps[0])))
byte zox_tst_seams_stage = 0;
int zox_tst_seams_step = 0;
int zox_tst_seams_settled = 0;
int zox_tst_seams_frames = 0;
entity zox_tst_seams_streamer = 0;
zox_tst_seam_result zox_tst_seams_total = { 0 };
int zox_tst_seams_bad_steps = 0;

static void zox_tst_seams_check_step(ecs* world, entity terrain) {
    zox_tst_seam_reset();
    zox_tst_seam_result step = { 0 };
    zox_tst_seam_check(world, terrain, 0, &step);
    zox_log("[tst] step [%i]: [%i] borders ([%i] between lods), [%i] seams in [%i] of them ([%i] between lods), "
        "[%i] hidden faces drawn, [%i] beside a chunk not ready (generating [%i], no materials [%i], a material unbuilt [%i])",
        zox_tst_seams_step, step.pairs, step.lod_pairs, step.missing, step.seam_pairs, step.missing_lod, step.extra, step.unready,
        step.unready_why[1], step.unready_why[2], step.unready_why[3]);
    zox_tst_seams_bad_steps += step.missing > 0;
    zox_tst_seams_total.pairs += step.pairs;
    zox_tst_seams_total.lod_pairs += step.lod_pairs;
    zox_tst_seams_total.missing += step.missing;
    zox_tst_seams_total.missing_lod += step.missing_lod;
    zox_tst_seams_total.extra += step.extra;
    zox_tst_seams_total.unready += step.unready;
    zox_tst_seams_total.unknown += step.unknown;
    zox_tst_seams_total.seam_pairs += step.seam_pairs;
    zox_tst_seam_reset();
}

byte zox_tst_stream_seams_ready(ecs* world) {
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) {
        return 0;
    }
    const float chunk = octree_size(zox_getv(terrain, NodeDepth)) * zox_getv(terrain, BlockScale);
    zox_tst_progress(zox_tst_seams_step * 10000 + zox_tst_seams_frames);
    zox_tst_seams_frames++;
    if (zox_tst_seams_stage == 0) {
        zox_tst_seams_streamer = spawn_streamer(world, prefab_streamer, terrain, (float3) { chunk * 0.5f, chunk, chunk * 0.5f });
        zox_setv(zox_tst_seams_streamer, StreamerLevel, 1);
        zox_tst_seams_stage = 1;
        return 0;
    }
    zox_tst_waiting = "the stream and its meshes to settle";
    zox_tst_seams_settled = zox_tst_terrain_busy(world, terrain) ? 0 : zox_tst_seams_settled + 1;
    if (zox_tst_seams_frames < 10 || zox_tst_seams_settled < 3) {
        return 0;
    }
    zox_tst_seams_check_step(world, terrain);
    if (zox_tst_seams_step >= zox_tst_seams_steps_count) {
        return 1;
    }
    const int3 move = zox_tst_seams_steps[zox_tst_seams_step];
    float3 position = zox_getv(zox_tst_seams_streamer, Position3D);
    position = float3_add(position, (float3) { move.x * chunk, move.y * chunk, move.z * chunk });
    zox_setv(zox_tst_seams_streamer, Position3D, position);
    zox_tst_seams_step++;
    zox_tst_seams_settled = 0;
    zox_tst_seams_frames = 0;
    return 0;
}

void zox_tst_stream_seams_check(ecs* world) {
    (void) world;
    const zox_tst_seam_result* t = &zox_tst_seams_total;
    zox_log("[tst] [metric] stream_seams [%s meshing] [%i] borders checked, [%i] between lods: [%i] seams ([%i] between lods) "
        "in [%i] borders over [%i] of [%i] checks, [%i] hidden faces drawn, [%i] beside chunks not ready, [%i] mid upload",
        zox_chunks3_gpu_meshing ? "gpu" : "cpu", t->pairs, t->lod_pairs, t->missing, t->missing_lod, t->seam_pairs,
        zox_tst_seams_bad_steps, zox_tst_seams_steps_count + 1, t->extra, t->unready, t->unknown);
    zox_tst_check(zox_tst_seams_step >= zox_tst_seams_steps_count, "the stream stopped at step [%i]", zox_tst_seams_step);
    zox_tst_check(t->pairs > 1000, "only [%i] borders checked", t->pairs);
    zox_tst_check(t->lod_pairs > 100, "only [%i] borders between lods, the lods weren't tested", t->lod_pairs);
    zox_tst_check(!t->missing, "[%i] border cells owed a face and drawn without one (holes), [%i] where lods meet",
        t->missing, t->missing_lod);
    // a settled stream: everything beside a ready chunk is ready, or almost (its edge's last generating)
    zox_tst_check(t->unready * 20 < t->pairs, "[%i] borders beside chunks not ready, of [%i]", t->unready, t->pairs);
}

void zox_tsts_stream_seams(ecs* world) {
    (void) world;
    zox_tst_add("stream_seams", zox_tst_stream_seams_ready, zox_tst_stream_seams_check, 240);
    zox_tst_set_setup("stream_seams", NULL, 1);
}
