// tst: perf_stream, measures streaming as the player moves across chunks, and the world's size
//  - after the player spawns, moves its character one chunk at a time in a straight line
//  - times each step until the terrain settles: streaming moved to the new chunk, and no chunks
//      are generating, building sides or meshes
//  - logs [tst] [metric] lines (step times, flecs tables, and the frames while stepping: frames.c's summary) to
//      compare runs, i.e. when optimizing
//      (software rendering in tests, so compare runs on the same machine)
//  - profiles every frame while stepping: the slowest frames and the systems making them (lag spikes)
//  - fails if a step never settles (streaming stuck)
//  - run: bash bsh/tst.sh perf_stream
#define zox_tst_perf_steps 6
#define zox_tst_perf_step_max_seconds 20.0
int zox_tst_perf_step = -1;     // -1 waiting for the player to spawn
double zox_tst_perf_step_time = 0;
double zox_tst_perf_total = 0;
double zox_tst_perf_slowest = 0;
double zox_tst_perf_fastest = 0;
zox_tst_frames zox_tst_perf_frames;   // every frame from the first step to the last settling
int zox_tst_perf_settled_frames = 0;
int zox_tst_perf_step_tables = 0;      // tables when the step began
int zox_tst_perf_tables_most = 0;      // the most tables one step made
int zox_tst_perf_tables_total = 0;
uintptr_t* zox_tst_perf_tables_before = NULL;  // tables when the step began
int zox_tst_perf_tables_before_count = 0;

int zox_tst_perf_queued = 0;       // tunk updates waiting in the stream queue

static inline int zox_tst_stream_queued(ecs* world) {
    return zox_valid(local_terrain) ? (int) zox_get(local_terrain, TerrainSpawnQueue)->count : 0;
}

// a terrain still busy: its stream queue, or its chunks (zox_tst_chunks_busy, lights/tst/chunk3_parent.c)
static inline int zox_tst_terrain_busy(ecs* world, entity terrain) {
    zox_tst_perf_queued = zox_valid(terrain) ? (int) zox_get(terrain, TerrainSpawnQueue)->count : 0;
    return zox_tst_perf_queued + zox_tst_chunks_busy(world, terrain);
}

// the game's terrain busy (perf_stream, perf_idle)
static inline int zox_tst_perf_busy(ecs* world) {
    return zox_tst_terrain_busy(world, local_terrain);
}

// tunks within zox_tst_near_tunks of the streamer not at their lod yet: missing, an old distance, or a chunk mid swap
//  (what the player sees lag: the land round them), doc/done/streaming.md
#define zox_tst_near_tunks 2
int zox_tst_near_missing = 0, zox_tst_near_distance = 0, zox_tst_near_states[5];   // why they're pending
int zox_tst_near_mesh = 0, zox_tst_near_lights = 0, zox_tst_near_mesh_lights = 0, zox_tst_near_blocked = 0;  // swaps waiting on
static inline int zox_tst_near_pending(ecs* world, int2 stream) {
    zox_tst_near_missing = zox_tst_near_distance = 0;
    zox_tst_near_mesh = zox_tst_near_lights = zox_tst_near_mesh_lights = zox_tst_near_blocked = 0;
    memset(zox_tst_near_states, 0, sizeof(zox_tst_near_states));
    if (!zox_valid(local_terrain)) {
        return 0;
    }
    const TunkLinks* tunks = zox_get(local_terrain, TunkLinks);
    int pending = 0;
    for (int x = -zox_tst_near_tunks; x <= zox_tst_near_tunks; x++)
    for (int z = -zox_tst_near_tunks; z <= zox_tst_near_tunks; z++) {
        const int2 position = { stream.x + x, stream.y + z };
        const int distance = int2_distance(stream, position);
        if (distance > zox_tst_near_tunks) {
            continue;
        }
        const entity tunk = int2_hashmap_get(tunks->value, position);
        if (!zox_valid(tunk)) {
            pending++;
            zox_tst_near_missing++;
            continue;
        }
        if (zox_getv(tunk, RenderDistance) != distance) {
            pending++;
            zox_tst_near_distance++;
            continue;
        }
        const Chunk3Stack* stack = zox_get(tunk, Chunk3Stack);
        for (int k = 0; k < stack->length; k++) {
            if (zox_valid(stack->value[k]) && zox_has(stack->value[k], ChunkLodDirty)) {
                pending++;
                const entity chunk = stack->value[k];
                zox_tst_near_states[zox_getv(chunk, ChunkLodDirty) % 5]++;
                const entity material = get_chunk_material(world, chunk, zox_block_material_solid);
                const entity mesh = material ? zox_get_link(world, material, PreparingMesh) : 0;
                if (zox_getv(chunk, ChunkLodDirty) == zox_chunk_lod_dirty_toggle && zox_valid(mesh)) {
                    zox_tst_near_mesh += zox_has(mesh, BuildMesh) || zox_has(mesh, MeshDirty) || !zox_has(mesh, MeshBuilt);
                    zox_tst_near_lights += zox_chunk_lighting_busy(world, chunk);
                    zox_tst_near_mesh_lights += zox_chunk_mesh_lighting_busy(world, mesh);
                    zox_tst_near_blocked += zox_has(chunk, GenerateChunk) || zox_has(chunk, BuildChunkSides);
                }
                break;
            }
        }
    }
    return pending;
}

byte zox_tst_perf_stream_ready(ecs* world) {
    if (zox_tst_perf_step == -1) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_log("[tst] [metric] perf_stream spawn_tables [%i]", zox_get_tables_count(world));
        zox_tst_perf_step = 0;
        zox_tst_perf_step_time = 0;
    }
    zox_tst_profile_frame(world);
    if (zox_tst_perf_step_time) {
        zox_tst_frames_tick(&zox_tst_perf_frames);
    }
    zox_tst_waiting = "streaming to settle after a step";
    // the walk over every chunk is the test's time, not the game's: left out of the frame times
    const double busy_start = get_time_ms();
    const int busy = zox_tst_perf_busy(world);
    zox_tst_frames_exclude(&zox_tst_perf_frames, get_time_ms() - busy_start);
    zox_tst_progress(zox_tst_perf_step * 100000.0 + busy);
    // the first load settles before the first step, so each step counts only its own work and tables
    if (!zox_tst_perf_step_time) {
        zox_tst_perf_settled_frames = busy ? 0 : zox_tst_perf_settled_frames + 1;
        if (zox_tst_perf_settled_frames < 3) {
            return 0;
        }
    }
    if (zox_tst_perf_step_time) {
        const double elapsed = zox_current_time - zox_tst_perf_step_time;
        // streaming noticed the move: the camera streams from the chunk it's in
        byte streamed = 0;
        const entity camera = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CameraLink) : 0;
        if (zox_valid(camera) && zox_has(camera, StreamPosition) && zox_valid(local_terrain)) {
            const float scale = zox_getv(local_terrain, BlockScale);
            const int length = octree_size(zox_getv(local_terrain, NodeDepth));
            const int3 camera_chunk = real_position_to_chunk_position(zox_getv(camera, Position3D), length, scale);
            const int3 stream = zox_getv(camera, StreamPosition);
            streamed = camera_chunk.x == stream.x && camera_chunk.z == stream.z && !zox_getv(camera, StreamDirty);
        }
        // settled for a few frames in a row
        zox_tst_perf_settled_frames = busy || !streamed ? 0 : zox_tst_perf_settled_frames + 1;
        if (zox_tst_perf_settled_frames < 3) {
            if (elapsed > zox_tst_perf_step_max_seconds) {
                zox_tst_check(0, "step [%i] didn't settle in [%.0fs], [%i] still busy: queued [%i] swapping [%i] generating [%i] sides [%i] meshes [%i] (build disabled [%i])",
                    zox_tst_perf_step, zox_tst_perf_step_max_seconds, busy, zox_tst_perf_queued, zox_tst_perf_swapping,
                    zox_tst_perf_generating, zox_tst_perf_siding, zox_tst_perf_meshing, zox_count_types(BuildDisabled));
                return 1;
            }
            return 0;
        }
        const int tables_made = zox_get_tables_count(world) - zox_tst_perf_step_tables;
        zox_log("[tst] [metric] perf_stream step [%i] settled in [%.3fs], tables made [%i]", zox_tst_perf_step, elapsed, tables_made);
        zox_tst_perf_tables_most = tables_made > zox_tst_perf_tables_most ? tables_made : zox_tst_perf_tables_most;
        zox_tst_perf_tables_total += tables_made;
        // what the step made, by family (the last step's, and any step making a lot)
        if (zox_tst_perf_tables_before && (zox_tst_perf_step == zox_tst_perf_steps - 1 || tables_made > 200)) {
            static char families[512][zox_table_family_length];
            static int counts[512];
            int total = 0;
            const int count = zox_new_table_families(world, zox_tst_perf_tables_before, zox_tst_perf_tables_before_count, families, counts, 512, &total);
            zox_log("[tst] [metric] perf_stream step [%i] made [%i] tables, in [%i] families", zox_tst_perf_step, total, count);
            for (int j = 0; j < count && j < 12; j++) {
                int i = 0;  // the biggest family left
                for (int k = 1; k < count; k++) {
                    i = counts[k] > counts[i] ? k : i;
                }
                if (!counts[i]) {
                    break;
                }
                const int length = (int) strlen(families[i]);
                zox_log("[tst] [metric] perf_stream new [%i]: %.110s ... %s", counts[i], families[i], families[i] + (length > 110 ? length - 110 : length));
                counts[i] = 0;
            }
        }
        zox_tst_perf_total += elapsed;
        if (elapsed > zox_tst_perf_slowest) {
            zox_tst_perf_slowest = elapsed;
        }
        if (!zox_tst_perf_fastest || elapsed < zox_tst_perf_fastest) {
            zox_tst_perf_fastest = elapsed;
        }
        zox_tst_perf_step++;
        if (zox_tst_perf_step >= zox_tst_perf_steps) {
            char label[128];
            zox_tst_frames_label(label, sizeof(label));
            zox_log("[tst] [metric] perf_stream %s: steps [%i] settle in seconds average [%.3f] fastest [%.3f] slowest [%.3f]",
                label, zox_tst_perf_steps, zox_tst_perf_total / zox_tst_perf_steps, zox_tst_perf_fastest, zox_tst_perf_slowest);
            zox_log("[tst] [metric] perf_stream steps [%i] average [%.3fs] slowest [%.3fs] tables [%i]",
                zox_tst_perf_steps, zox_tst_perf_total / zox_tst_perf_steps, zox_tst_perf_slowest, zox_get_tables_count(world));
            zox_tst_frames_log(&zox_tst_perf_frames, "perf_stream");
            zox_tst_frames_dispose(&zox_tst_perf_frames);
            zox_tst_profile_log(world, "perf_stream");
            return 1;
        }
    }
    // moves the character one chunk along x
    const entity character = zox_tst_player_character(world);
    const entity terrain = local_terrain;
    if (!character || !zox_valid(terrain)) {
        zox_tst_check(0, "the player's character or terrain is gone");
        return 1;
    }
    const float scale = zox_getv(terrain, BlockScale);
    const float chunk_length = octree_size(zox_getv(terrain, NodeDepth)) * scale;
    float3 position = zox_getv(character, Position3D);
    position.x += chunk_length;
    position.y += scale * 2;
    zox_setv(character, Position3D, position);
    if (!zox_tst_perf_step_time) {
        zox_tst_frames_begin(&zox_tst_perf_frames);
    }
    zox_tst_perf_step_time = zox_current_time;
    zox_tst_perf_settled_frames = 0;
    zox_tst_perf_step_tables = zox_get_tables_count(world);
    if (zox_tst_perf_tables_before) {
        free(zox_tst_perf_tables_before);
    }
    zox_tst_perf_tables_before = zox_tables_snapshot(world, &zox_tst_perf_tables_before_count);
    return 0;
}

// the table families with the most tables (a pair with many targets fragments)
static inline void zox_tst_log_child_tables(ecs* world, const char* name) {
    static char families[256][zox_table_family_length];
    static int counts[256];
    int total = 0;
    const int count = zox_table_families(world, families, counts, 256, &total);
    zox_log("[tst] [metric] %s tables [%i], counted [%i], families [%i]", name, zox_get_tables_count(world), total, count);
    for (int i = 0; i < count && i < 10; i++) {
        zox_log("[tst] [metric] %s family [%i] tables: %s", name, counts[i], families[i]);
    }
}

void zox_tst_perf_stream_check(ecs* world) {
    zox_tst_log_child_tables(world, "perf_stream");
}

// tst: tables_stream, streaming across chunks doesn't make many flecs tables
//  - walks across chunks like perf_stream, counts the tables each chunk step makes
//  - each new table is merge work and notifies every observer (lag spikes crossing chunks),
//      ChildOf children get tables per parent, so chunks' children must use non fragmenting parents
//  - logs the table families with the most tables (pairs with many targets), to find the next to fix
//  - run: bash bsh/tst.sh tables_stream
// npcs labels, glyphs, stats, skills and items are non fragmenting children now (they made 400 to 2500
//  tables a step as ChildOf children), bones, the combined vox and renderer children too (a town's step made
//  727, 142 now on zox_tst_seed=666), a guard against it getting worse, lower it as more stop fragmenting
//  (doc/children.md)
#define zox_tst_tables_per_step_max 200

void zox_tst_tables_stream_check(ecs* world) {
    zox_tst_log_child_tables(world, "tables_stream");
    zox_log("[tst] [metric] tables_stream steps [%i] tables made [%i], most in a step [%i]",
        zox_tst_perf_steps, zox_tst_perf_tables_total, zox_tst_perf_tables_most);
    zox_tst_check(zox_tst_perf_tables_most <= zox_tst_tables_per_step_max,
        "a chunk step made [%i] tables, over [%i]", zox_tst_perf_tables_most, zox_tst_tables_per_step_max);
}

// tst: perf_fly, the player flies across the terrain like playing, not stepping and waiting like perf_stream
//  - diagonal (two chunk borders at once), about a chunk a second, for zox_tst_fly_seconds, streaming never
//    settles, so its spikes stack like they do in play
//  - profiles every frame, the slowest frames and systems, run in release at default settings for real numbers:
//      bash bsh/build.sh zoxel --release, then the release binary with --tst perf_fly --profile --nosaves
//  - run: bash bsh/tst.sh perf_fly --stress (tests default to --tiny terrain)
#define zox_tst_fly_seconds 20.0
#define zox_tst_fly_chunks_per_second 1.0
double zox_tst_fly_start = 0;
float zox_tst_fly_height = 0;
// near lod latency: from crossing into a tunk until the tunks near the player are at their lod (zox_tst_near_pending)
double zox_tst_fly_crossed = 0;     // the oldest crossing not caught up with, 0 once caught up
byte zox_tst_fly_streaming = 0;
int2 zox_tst_fly_stream;
int zox_tst_fly_crossings = 0, zox_tst_fly_caught = 0;
double zox_tst_fly_latency_total = 0, zox_tst_fly_latency_most = 0;

// once a second: the queue and its slots by kind; each crossing: how long the land near the player lags
static inline void zox_tst_fly_stream_metrics(ecs* world) {
    const entity camera = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CameraLink) : 0;
    if (!zox_valid(camera) || !zox_has(camera, StreamPosition)) {
        return;
    }
    const int3 stream3 = zox_getv(camera, StreamPosition);
    const int2 stream = { stream3.x, stream3.z };
    if (!zox_tst_fly_streaming) {
        zox_tst_fly_streaming = 1;
        zox_tst_fly_stream = stream;
    } else if (stream.x != zox_tst_fly_stream.x || stream.y != zox_tst_fly_stream.y) {
        zox_tst_fly_stream = stream;
        zox_tst_fly_crossings++;
        if (!zox_tst_fly_crossed) {
            zox_tst_fly_crossed = zox_current_time;
        }
    }
    const int pending = zox_tst_near_pending(world, stream);
    if (zox_tst_fly_crossed && !pending) {
        const double latency = zox_current_time - zox_tst_fly_crossed;
        zox_tst_fly_latency_total += latency;
        zox_tst_fly_latency_most = latency > zox_tst_fly_latency_most ? latency : zox_tst_fly_latency_most;
        zox_tst_fly_caught++;
        zox_tst_fly_crossed = 0;
    }
    static double logged = 0;
    static int lod = 0, distance = 0, spawn = 0;
    if (zox_current_time - logged >= 1.0) {
        logged = zox_current_time;
        zox_log("[tst] [metric] perf_fly queue [%i] slots lod [%i] distance [%i] spawn [%i] near pending [%i] (missing [%i] distance [%i] swaps generating [%i] spawning [%i] toggling [%i]: mesh [%i] lights [%i] mesh lights [%i] generating or sides [%i])",
            zox_tst_stream_queued(world), stream_slots_lod - lod, stream_slots_distance - distance, stream_slots_spawn - spawn, pending,
            zox_tst_near_missing, zox_tst_near_distance, zox_tst_near_states[1] + zox_tst_near_states[2], zox_tst_near_states[3], zox_tst_near_states[4],
            zox_tst_near_mesh, zox_tst_near_lights, zox_tst_near_mesh_lights, zox_tst_near_blocked);
        lod = stream_slots_lod;
        distance = stream_slots_distance;
        spawn = stream_slots_spawn;
    }
}

byte zox_tst_perf_fly_ready(ecs* world) {
    if (!zox_tst_fly_start) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_fly_start = zox_current_time;
        const entity character = zox_tst_player_character(world);
        zox_tst_fly_height = zox_getv(character, Position3D).y + 8 * zox_getv(local_terrain, BlockScale);
    }
    zox_tst_profile_frame(world);
    zox_tst_fly_stream_metrics(world);
    const double elapsed = zox_current_time - zox_tst_fly_start;
    // characters alive (the player and npcs: InsideBlock, set on every character), once a second (counting walks
    //  every table)
    static int characters_peak = 0;
    static double characters_counted = 0;
    if (zox_current_time - characters_counted >= 1.0) {
        characters_counted = zox_current_time;
        const int characters = zox_count_types(InsideBlock);
        if (characters > characters_peak) {
            characters_peak = characters;
        }
    }
    if (elapsed >= zox_tst_fly_seconds) {
        zox_log("[tst] [metric] perf_fly characters peak [%i]", characters_peak);
    }
    zox_tst_waiting = "flying across the terrain";
    zox_tst_progress(elapsed);
    if (elapsed >= zox_tst_fly_seconds) {
        return 1;
    }
    const entity character = zox_tst_player_character(world);
    if (!character || !zox_valid(local_terrain)) {
        zox_tst_check(0, "the player's character or terrain is gone");
        return 1;
    }
    const float scale = zox_getv(local_terrain, BlockScale);
    const float chunk_length = octree_size(zox_getv(local_terrain, NodeDepth)) * scale;
    const float distance = elapsed * zox_tst_fly_chunks_per_second * chunk_length * 0.7071f;
    // from where it spawned, so frame time doesn't change the path
    static float3 origin;
    static byte has_origin = 0;
    if (!has_origin) {
        origin = zox_getv(character, Position3D);
        has_origin = 1;
    }
    const float3 position = { origin.x + distance, zox_tst_fly_height, origin.z + distance };
    zox_setv(character, Position3D, position);
    zox_setv(character, Velocity3D, float3_zero);
    return 0;
}

void zox_tst_perf_fly_check(ecs* world) {
    // the number to lower: how long the land round the player lags behind a crossing
    zox_log("[tst] [metric] perf_fly near lod latency: crossings [%i] caught up [%i] average [%.2fs] most [%.2fs] behind at the end [%.2fs]",
        zox_tst_fly_crossings, zox_tst_fly_caught, zox_tst_fly_caught ? zox_tst_fly_latency_total / zox_tst_fly_caught : 0,
        zox_tst_fly_latency_most, zox_tst_fly_crossed ? zox_current_time - zox_tst_fly_crossed : 0);
    zox_tst_profile_log(world, "perf_fly");
}

void zox_tsts_perf(ecs* world) {
    zox_tst_add("perf_stream", zox_tst_perf_stream_ready, zox_tst_perf_stream_check, 180);
    zox_tst_add("perf_fly", zox_tst_perf_fly_ready, zox_tst_perf_fly_check, 120);
    zox_tst_add("tables_stream", zox_tst_perf_stream_ready, zox_tst_tables_stream_check, 200);
}
