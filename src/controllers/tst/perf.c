// tst: perf_stream, measures streaming as the player moves across chunks, and the world's size
//  - after the player spawns, moves its character one chunk at a time in a straight line
//  - times each step until the terrain settles: streaming moved to the new chunk, and no chunks
//      are generating, building sides or meshes
//  - logs [tst] [metric] lines (step times, flecs tables) to compare runs, i.e. when optimizing
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
int zox_tst_perf_settled_frames = 0;
int zox_tst_perf_step_tables = 0;      // tables when the step began
int zox_tst_perf_tables_most = 0;      // the most tables one step made
int zox_tst_perf_tables_total = 0;
uintptr_t* zox_tst_perf_tables_before = NULL;  // tables when the step began
int zox_tst_perf_tables_before_count = 0;

// the terrain's chunks still generating or building sides, and their meshes still building
//  (counting the tags world wide also counts prefabs and other voxel chunks, i.e. characters)
int zox_tst_perf_generating = 0;
int zox_tst_perf_siding = 0;
int zox_tst_perf_meshing = 0;
static inline int zox_tst_perf_busy(ecs* world) {
    zox_tst_perf_generating = 0;
    zox_tst_perf_siding = 0;
    zox_tst_perf_meshing = 0;
    if (!zox_valid(local_terrain)) {
        return 0;
    }
    iter it2 = zox_children(world, local_terrain);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity chunk = it2.entities[j];
            if (!zox_has(chunk, Chunk3)) {
                continue;
            }
            zox_tst_perf_generating += zox_has(chunk, GenerateChunk) && zox_getv(chunk, GenerateChunk);
            zox_tst_perf_siding += zox_has(chunk, BuildChunkSides);
            iter it3 = zox_children(world, chunk);
            while (zox_children_next(it3)) {
                for (int k = 0; k < it3.count; k++) {
                    zox_tst_perf_meshing += zox_has(it3.entities[k], ChunkMesh) && zox_has(it3.entities[k], BuildMesh);
                }
            }
        }
    }
    return zox_tst_perf_generating + zox_tst_perf_siding + zox_tst_perf_meshing;
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
    zox_tst_waiting = "streaming to settle after a step";
    const int busy = zox_tst_perf_busy(world);
    zox_tst_progress(zox_tst_perf_step * 100000.0 + busy);
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
                zox_tst_check(0, "step [%i] didn't settle in [%.0fs], [%i] still busy: generating [%i] sides [%i] meshes [%i] (build disabled [%i])",
                    zox_tst_perf_step, zox_tst_perf_step_max_seconds, busy,
                    zox_tst_perf_generating, zox_tst_perf_siding, zox_tst_perf_meshing, zox_count_types(BuildDisabled));
                return 1;
            }
            return 0;
        }
        const int tables_made = zox_get_tables_count(world) - zox_tst_perf_step_tables;
        zox_log("[tst] [metric] perf_stream step [%i] settled in [%.3fs], tables made [%i]", zox_tst_perf_step, elapsed, tables_made);
        zox_tst_perf_tables_most = tables_made > zox_tst_perf_tables_most ? tables_made : zox_tst_perf_tables_most;
        zox_tst_perf_tables_total += tables_made;
        // what the step made, by family (the last step's, logged once)
        if (zox_tst_perf_tables_before && zox_tst_perf_step == zox_tst_perf_steps - 1) {
            static char families[512][zox_table_family_length];
            static int counts[512];
            int total = 0;
            const int count = zox_new_table_families(world, zox_tst_perf_tables_before, zox_tst_perf_tables_before_count, families, counts, 512, &total);
            zox_log("[tst] [metric] perf_stream the last step made [%i] tables, in [%i] families", total, count);
            for (int i = 0; i < count && i < 12; i++) {
                const int length = (int) strlen(families[i]);
                zox_log("[tst] [metric] perf_stream new [%i]: %.110s ... %s", counts[i], families[i], families[i] + (length > 110 ? length - 110 : length));
            }
        }
        zox_tst_perf_total += elapsed;
        if (elapsed > zox_tst_perf_slowest) {
            zox_tst_perf_slowest = elapsed;
        }
        zox_tst_perf_step++;
        if (zox_tst_perf_step >= zox_tst_perf_steps) {
            zox_log("[tst] [metric] perf_stream steps [%i] average [%.3fs] slowest [%.3fs] tables [%i]",
                zox_tst_perf_steps, zox_tst_perf_total / zox_tst_perf_steps, zox_tst_perf_slowest, zox_get_tables_count(world));
            zox_tst_profile_log(world, "perf_stream");
            return 1;
        }
    }
    // moves the character one chunk along x
    const entity character = zox_tst_spawn_character(world);
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
//  tables a step as ChildOf children), about 100 a step now (block decor, vox instances),
//  a guard against it getting worse, lower it as they stop fragmenting (doc/children.md)
#define zox_tst_tables_per_step_max 300

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

byte zox_tst_perf_fly_ready(ecs* world) {
    if (!zox_tst_fly_start) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_fly_start = zox_current_time;
        const entity character = zox_tst_spawn_character(world);
        zox_tst_fly_height = zox_getv(character, Position3D).y + 8 * zox_getv(local_terrain, BlockScale);
    }
    zox_tst_profile_frame(world);
    const double elapsed = zox_current_time - zox_tst_fly_start;
    zox_tst_waiting = "flying across the terrain";
    zox_tst_progress(elapsed);
    if (elapsed >= zox_tst_fly_seconds) {
        return 1;
    }
    const entity character = zox_tst_spawn_character(world);
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
    zox_tst_profile_log(world, "perf_fly");
}
