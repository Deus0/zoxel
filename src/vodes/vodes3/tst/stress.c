// tst: vox_instances_stress, many instanced vox meshes drawn at once (vox_instance_render_system: grass, minivoxes)
//  - a realm of its own (world_only.c, no game, no terrain shown), its tall grass block's most detailed mesh
//  - in rounds, N instances of it in a grid in front of a viewport camera (all in view), held for frames
//  - logs a round's render system time a frame (its cpu side: gathering, uploads, draw calls) and the frame's
//    (software rendering in tests: the gpu side is slow here, compare runs on one machine; --gpu for a real one)
//  - checks every instance is drawn each frame (the last round: half behind the camera, culled), and no gl errors (a mesh's instances past the uniform buffer's
//    size (zox_get_safe_ubo_size) overflowed it)
//  - run: bash bsh/tst.sh vox_instances_stress --gpu --release for real numbers (software: the least detailed mesh) (zox_tst_stress_counts=1000,8000 for other rounds)
#define zox_tst_stress_rounds_max 8
#define zox_tst_stress_warmup 10
#define zox_tst_stress_frames 40
int zox_tst_stress_counts[zox_tst_stress_rounds_max] = { 1000, 4000, 16000, 16000 };
int zox_tst_stress_rounds = 4;   // the last with half behind the camera
byte zox_tst_stress_stage = 0;
int zox_tst_stress_round = 0, zox_tst_stress_frame = 0;
entity zox_tst_stress_mesh = 0, zox_tst_stress_camera = 0;
entity* zox_tst_stress_instances = NULL;
int zox_tst_stress_count = 0;
double zox_tst_stress_render_begin = 0, zox_tst_stress_wall_begin = 0;
uint zox_tst_stress_drawn_begin = 0;
double zox_tst_stress_render_ms[zox_tst_stress_rounds_max], zox_tst_stress_frame_ms[zox_tst_stress_rounds_max];
float zox_tst_stress_drawn[zox_tst_stress_rounds_max];
byte zox_tst_stress_gl_errors[zox_tst_stress_rounds_max];

// the realm's first biome's tall grass, its most detailed mesh on the gpu (least in software), 0 until there
static inline entity zox_tst_stress_find_mesh(ecs* world, entity realm) {
    // a biome's (each has its own, vegetation/sys/chunks.c)
    const BiomeLinks* biomes = zox_has(realm, BiomeLinks) ? zox_get(realm, BiomeLinks) : NULL;
    const entity biome = biomes && biomes->length ? biomes->value[0] : 0;
    const entity block = zox_valid(biome) ? zox_get_child_by_id(world, biome, zox_id(BlockGrass)) : 0;
    const entity group = zox_valid(block) ? zox_get_link(world, block, ModelLink) : 0;
    entity models[8];
    const uint count = zox_valid(group) ? zox_get_children(world, group, models, 8) : 0;
    if (!count || !zox_has(models[0], VoxLods)) {
        return 0;
    }
    // software rendering (tests without --gpu): its least detailed, the most detailed took seconds a frame there
    const byte software = getenv("LIBGL_ALWAYS_SOFTWARE") != NULL;
    for (int k = 0; k < model_lods_max_length; k++) {
        const int d = software ? k : model_lods_max_length - 1 - k;
        const entity mesh = get_model_lod_mesh(world, models[0], d);
        if (zox_valid(mesh) && zox_has(mesh, MeshGPULink) && zox_getv(mesh, MeshGPULink).x &&
            mesh_index_count(world, mesh))
        {
            return mesh;
        }
    }
    return 0;
}

// a round's instances: a square grid ahead of the camera (it looks along -z from the origin), half a block apart,
//  far enough out to be all in view; behind: every other one behind the camera instead (culled)
static inline void zox_tst_stress_spawn(ecs* world, int count, byte behind) {
    zox_tst_stress_instances = malloc(sizeof(entity) * count);
    const int side = (int) ceilf(sqrtf((float) count));
    const float spacing = 0.25f;
    for (int i = 0; i < count; i++) {
        float3 p = { (i % side - side * 0.5f) * spacing, -3, -20 - (i / side) * spacing };
        if (behind && i % 2) {
            p.z = -p.z;
        }
        const entity e = zox_new();
        zox_add(e, RendererInstance);
        zox_set(e, TransformMatrix, { float4x4_position(p) });
        zox_set(e, RenderDisabled, { 0 });
        zox_set(e, Brightness, { 1 });
        zox_set_link(world, e, InstanceLink, zox_tst_stress_mesh);
        zox_tst_stress_instances[i] = e;
    }
    zox_tst_stress_count = count;
}

static inline void zox_tst_stress_clear(ecs* world) {
    for (int i = 0; i < zox_tst_stress_count; i++) {
        zox_delete(zox_tst_stress_instances[i]);
    }
    free(zox_tst_stress_instances);
    zox_tst_stress_instances = NULL;
    zox_tst_stress_count = 0;
}

// the last of the default rounds puts half behind the camera
static inline byte zox_tst_stress_behind_round() {
    return !getenv("zox_tst_stress_counts") && zox_tst_stress_round == zox_tst_stress_rounds - 1;
}

byte zox_tst_vox_instances_stress_ready(ecs* world) {
    if (!zox_tst_world_ready(world)) {
        return 0;
    }
    zox_tst_progress(zox_tst_stress_round * 1000 + zox_tst_stress_frame);
    if (zox_tst_stress_stage == 0) {
        zox_tst_waiting = "the tall grass mesh on the gpu";
        zox_tst_stress_mesh = zox_tst_stress_find_mesh(world, zox_tst_world_realm);
        if (!zox_tst_stress_mesh) {
            return 0;
        }
        const char* counts = getenv("zox_tst_stress_counts");
        if (counts) {
            zox_tst_stress_rounds = 0;
            for (const char* c = counts; *c && zox_tst_stress_rounds < zox_tst_stress_rounds_max; c++) {
                zox_tst_stress_counts[zox_tst_stress_rounds++] = atoi(c);
                while (c[1] && c[1] != ',') {
                    c++;
                }
                if (c[1] == ',') {
                    c++;
                }
            }
        }
        const float3 eye = { 0, 0, 0 };
        zox_tst_stress_camera = spawn_viewport_camera(world, main_app, eye,
            camera_look_at(eye, (float3) { 0, -3, -28 }), 90, viewport_grid_cell(0, 1, 1));
        nohud = 1;
        const entity game_view = zox_valid(main_cameras[0]) ? zox_get_link(world, main_cameras[0], RenderTextureLink) : 0;
        if (zox_valid(game_view)) {
            zox_setv(game_view, RenderDisabled, 1);
        }
        zox_log("[tst] vox_instances_stress: a draw call holds [%i] instances (the uniform buffer), the mesh has [%i] triangles",
            zox_get_safe_ubo_size(), mesh_index_count(world, zox_tst_stress_mesh) / 3);
        zox_tst_stress_stage = 1;
        return 0;
    }
    zox_tst_waiting = "the rounds of instances";
    // the game's camera far under them, so only the test's draws them (each camera draws what it sees)
    if (zox_valid(main_cameras[0])) {
        zox_setv(main_cameras[0], Position3D, ((float3) { 0, -10000, 0 }));
    }
    if (!zox_tst_stress_count) {
        check_opengl_error_unlogged();  // clear earlier ones, this round's own after
        zox_tst_stress_spawn(world, zox_tst_stress_counts[zox_tst_stress_round], zox_tst_stress_behind_round());
        zox_tst_stress_frame = 0;
        return 0;
    }
    zox_tst_stress_frame++;
    if (zox_tst_stress_frame == zox_tst_stress_warmup) {
        zox_tst_stress_render_begin = zox_vox_instance_render_ms;
        zox_tst_stress_wall_begin = get_time_ms();
        zox_tst_stress_drawn_begin = zox_drawn_vox_instances;
    }
    if (zox_tst_stress_frame < zox_tst_stress_warmup + zox_tst_stress_frames) {
        return 0;
    }
    const int r = zox_tst_stress_round;
    zox_tst_stress_render_ms[r] = (zox_vox_instance_render_ms - zox_tst_stress_render_begin) / zox_tst_stress_frames;
    zox_tst_stress_frame_ms[r] = (get_time_ms() - zox_tst_stress_wall_begin) / zox_tst_stress_frames;
    zox_tst_stress_drawn[r] = (zox_drawn_vox_instances - zox_tst_stress_drawn_begin) / (float) zox_tst_stress_frames;
    zox_tst_stress_gl_errors[r] = check_opengl_error_unlogged();
    zox_log("[tst] [metric] vox_instances_stress [%i] instances: render system [%.2f]ms a frame, frame [%.1f]ms, drawn [%.0f] a frame",
        zox_tst_stress_count, zox_tst_stress_render_ms[r], zox_tst_stress_frame_ms[r], zox_tst_stress_drawn[r]);
    zox_tst_stress_clear(world);
    zox_tst_stress_round++;
    return zox_tst_stress_round >= zox_tst_stress_rounds;
}

void zox_tst_vox_instances_stress_check(ecs* world) {
    for (int r = 0; r < zox_tst_stress_rounds; r++) {
        // drawn counts every instance a camera draws, the test's camera sees them all (the main one is off)
        // all in view drawn; half behind: those culled
        const byte behind = !getenv("zox_tst_stress_counts") && r == zox_tst_stress_rounds - 1;
        const float expected = behind ? zox_tst_stress_counts[r] * 0.5f : zox_tst_stress_counts[r];
        zox_tst_check(fabsf(zox_tst_stress_drawn[r] - expected) < 1, "[%i] instances%s: [%.0f] drawn a frame, not [%.0f]",
            zox_tst_stress_counts[r], behind ? " (half behind)" : "", zox_tst_stress_drawn[r], expected);
        zox_tst_check(!zox_tst_stress_gl_errors[r], "[%i] instances: gl errors (the uniform buffer holds [%i])",
            zox_tst_stress_counts[r], zox_get_safe_ubo_size());
    }
    if (zox_valid(zox_tst_stress_camera)) {
        zox_delete(zox_tst_stress_camera);
    }
}

void zox_tsts_vox_instances_stress() {
    zox_tst_add("vox_instances_stress", zox_tst_vox_instances_stress_ready, zox_tst_vox_instances_stress_check, 180);
    zox_tst_set_setup("vox_instances_stress", NULL, 1);
}
