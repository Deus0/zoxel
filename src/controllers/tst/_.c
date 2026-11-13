#include "links.c"
#include "wheel.c"
#include "split.c"
#include "punch.c"
#include "pop.c"
#include "quest.c"
#include "quest_dialogue.c"
#include "npc_stats.c"
#include "hit_sound_links.c"
#include "clothes.c"
#include "lod_chunk_lights.c"
#include "npc_skeletons.c"
#include "npc_glance.c"
#include "npc_attacks.c"
#include "player_respawn.c"
#include "footsteps.c"
#include "arms_in_step.c"
#include "pipeline_syncs.c"
#include "lines_render.c"
#include "town_maps.c"
#include "clouds.c"
#include "head_camera.c"
#include "vegetation_chunks.c"
#include "music.c"
// tst: player_spawn, a new realm spawns the player's character standing in the terrain
//  - waits for the player's character, then lets it settle for a few seconds
//  - the character's block is air (not stuck in the ground) and it hasn't fallen out of the world
//  - it stands on ground: a solid block a few blocks under it
//  - run: bash bsh/tst.sh player_spawn (intermittent bugs: --repeat 5)
const double zox_tst_spawn_settle = 3.0;
double zox_tst_spawn_seen = 0;
double zox_tst_spawn_start = 0;

// the voxel at a world block position, -1 if its chunk isn't loaded
static inline int zox_tst_terrain_voxel(ecs* world, entity terrain, int3 block) {
    return terrain_voxel_at(world, terrain, block);
}

// logs the state of the camera's terrain column, so a stalled spawn shows what it waits on
void zox_tst_spawn_dump_column(ecs* world, entity terrain, int3 column) {
    // near chunks stuck generating, and their tunk's generation state
    int stuck = 0;
    iter it2 = zox_children(world, terrain);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity c = it2.entities[j];
            if (!zox_has(c, Chunk3) || !zox_has(c, GenerateChunk) || !zox_has(c, RenderDistance) || zox_getv(c, RenderDistance) > terrain_lod_near) {
                continue;
            }
            if (stuck++ >= 24) {
                continue;
            }
            const entity t = zox_get_link(world, c, TunkLink);
            const int3 p = zox_getv(c, ChunkPosition);
            zox_log("[tst]   generating [%s] at [%ix%ix%i] state [%i] depth [%i] distance [%i] tunk [%s] generate_tunk [%i] tunk_lod [%i]",
                zox_getn(c), p.x, p.y, p.z, zox_getv(c, GenerateChunk), zox_getv(c, NodeDepth), zox_getv(c, RenderDistance),
                zox_valid(t) ? zox_getn(t) : "none",
                zox_valid(t) && zox_has(t, GenerateTunk) ? zox_getv(t, GenerateTunk) : -1,
                zox_valid(t) && zox_has(t, TunkLod) ? zox_getv(t, TunkLod) : -1);
        }
    }
    zox_log("[tst]   near chunks generating [%i]", stuck);
    const TerrainSpawnQueue* queue = zox_get(terrain, TerrainSpawnQueue);
    const TunkLinks* tunks = zox_get(terrain, TunkLinks);
    const ChunkLinks* chunks = zox_get(terrain, ChunkLinks);
    const entity tunk = int2_hashmap_get(tunks->value, (int2) { column.x, column.z });
    zox_log("[tst] column [%ix%i] queue [%i] tunks [%i] chunks [%i] tunk [%s] valid [%i] loading [%i]",
        column.x, column.z, (int) queue->count, count_int2_hashmap(tunks->value), count_int3_hashmap(chunks->value),
        zox_valid(tunk) ? zox_getn(tunk) : "none", zox_valid(tunk), zox_has(terrain, Loading));
    // near columns with no tunk, i.e. never queued or dropped from the queue
    for (int dx = -terrain_lod_near; dx <= terrain_lod_near; dx++) {
        for (int dz = -terrain_lod_near; dz <= terrain_lod_near; dz++) {
            const int2 p2 = (int2) { column.x + dx, column.z + dz };
            if (!zox_valid(int2_hashmap_get(tunks->value, p2))) {
                zox_log("[tst]   near column [%ix%i] has no tunk (queued [%i])", p2.x, p2.y, get_TerrainSpawnQueue((TerrainSpawnQueue*) queue, p2) != NULL);
            }
        }
    }
    for (int k = 0; k < (int) queue->count && k < 5; k++) {
        const TerrainSpawnUpdate u = queue->ptr[k];
        zox_log("[tst]   queued [%ix%i] distance [%i] level [%i] tunk [%llu]", u.position.x, u.position.y, u.distance, u.level, (unsigned long long) u.tunk);
    }
    for (int y = render_distance_y; y >= -render_distance_y; y--) {
        const entity chunk = int3_hashmap_get(chunks->value, (int3) { column.x, y, column.z });
        if (!zox_valid(chunk)) {
            zox_log("[tst]   chunk y [%i] missing (hashmap [%llu])", y, (unsigned long long) chunk);
            continue;
        }
        int solid = 0;
        const byte depth = zox_getv(chunk, NodeDepth);
        const int length = octree_size(depth);
        const VoxelNode* voxels = zox_get(chunk, VoxelNode);
        for (int x = 0; x < length; x++) for (int yy = 0; yy < length; yy++) for (int z = 0; z < length; z++) {
            solid += getv_VoxelNode(voxels, depth, (byte3) { x, yy, z }) != 0;
        }
        zox_log("[tst]   chunk y [%i] [%s] depth [%i] solid [%i/%i] generate [%i]",
            y, zox_getn(chunk), depth, solid, length * length * length,
            zox_has(chunk, GenerateChunk) ? zox_getv(chunk, GenerateChunk) : -1);
    }
}

static inline entity zox_tst_spawn_character(ecs* world) {
    const entity player = dbg_player;
    if (!zox_valid(player)) {
        return 0;
    }
    const entity character = zox_get_link(world, player, CharacterLink);
    return zox_valid(character) && zox_has(character, Position3D) ? character : 0;
}

byte zox_tst_player_spawn_ready(ecs* world) {
    if (!zox_tst_spawn_start) {
        zox_tst_spawn_start = zox_current_time;
    }
    zox_tst_waiting = "the player's character to spawn in the terrain";
    if (!zox_tst_spawn_character(world)) {
        // streaming state in the waiting message, so a stall shows where it's stuck
        static char waiting[256];
        const entity camera = zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CameraLink) : 0;
        if (zox_valid(camera) && zox_valid(local_terrain) && zox_has(camera, StreamPosition)) {
            const float scale = zox_getv(local_terrain, BlockScale);
            const int length = octree_size(zox_getv(local_terrain, NodeDepth));
            const int3 camera_chunk = real_position_to_chunk_position(zox_getv(camera, Position3D), length, scale);
            const int3 stream = zox_getv(camera, StreamPosition);
            snprintf(waiting, sizeof(waiting),
                "the player's character to spawn (camera chunk [%ix%ix%i] stream [%ix%ix%i] dirty [%i] level [%i] player state [%i] blocked [%s] lod far [%i] y [%i] stream [%s])",
                camera_chunk.x, camera_chunk.y, camera_chunk.z,
                stream.x, stream.y, stream.z,
                zox_getv(camera, StreamDirty),
                zox_getv(camera, StreamerLevel),
                zox_getv(dbg_player, PlayerState),
                zox_player_spawn_blocked, terrain_lod_far, render_distance_y, zox_stream_end_status);
            zox_tst_waiting = waiting;
            // once, when streaming stopped: why the camera's column has no spawn position
            //  - once per test, 15s in (a stall fails the test 20s after its last progress)
            static byte dumped = 0;
            if (!dumped && zox_current_time - zox_tst_spawn_start > 15) {
                dumped = 1;
                zox_tst_spawn_dump_column(world, local_terrain, camera_chunk);
            }
        }
        // progress: chunks streamed in, and chunks finishing generation
        if (zox_valid(local_terrain) && zox_has(local_terrain, ChunkLinks)) {
            const ChunkLinks* chunks = zox_get(local_terrain, ChunkLinks);
            const int count = count_int3_hashmap(chunks->value);
            const int generating = zox_count_types(GenerateChunk);
            zox_tst_progress(count * 100000.0 + generating);
        }
        return 0;
    }
    if (!zox_tst_spawn_seen) {
        zox_tst_spawn_seen = zox_current_time;
        zox_log("[tst] player character spawned after [%.1fs]", zox_tst_spawn_seen - zox_tst_spawn_start);
    }
    zox_tst_waiting = "the player's character to settle";
    return zox_current_time - zox_tst_spawn_seen >= zox_tst_spawn_settle;
}

void zox_tst_player_spawn_check(ecs* world) {
    // the terrain's meshes matched their sides when lit (the mesh builder waits for sides)
    zox_tst_check(zox_smooth_lights_errors == 0, "[%u] Smooth Lights Colors Past Limits errors", zox_smooth_lights_errors);
    const entity character = zox_tst_spawn_character(world);
    zox_tst_check(character, "player character is gone after settling");
    if (!character) {
        return;
    }
    const entity terrain = local_terrain;
    zox_tst_check(zox_valid(terrain), "no terrain");
    if (!zox_valid(terrain)) {
        return;
    }
    const float3 position = zox_getv(character, Position3D);
    const float scale = zox_getv(terrain, BlockScale);
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    const float lowest = -(render_distance_y + 1) * length * scale;
    zox_log("[tst] character [%s] at [%.1fx%.1fx%.1f], world bottom [%.1f]",
        zox_getn(character), position.x, position.y, position.z, lowest);
    zox_tst_check(isfinite(position.x) && isfinite(position.y) && isfinite(position.z), "character position isn't finite");
    zox_tst_check(position.y > lowest, "character fell out of the world, y [%.1f] below [%.1f]", position.y, lowest);
    // the block the character stands in is air
    const int3 block = real_position_to_block_position(position, scale);
    const int3 chunk_position = block_position_to_chunk_position(block, depth);
    const ChunkLinks* chunks = zox_get(terrain, ChunkLinks);
    const entity chunk = int3_hashmap_get(chunks->value, chunk_position);
    zox_tst_check(zox_valid(chunk), "no chunk under the character at [%ix%ix%i]", chunk_position.x, chunk_position.y, chunk_position.z);
    if (!zox_valid(chunk)) {
        return;
    }
    const byte3 local = {
        block.x - chunk_position.x * length,
        block.y - chunk_position.y * length,
        block.z - chunk_position.z * length
    };
    const VoxelNode* voxels = zox_get(chunk, VoxelNode);
    const byte voxel = getv_VoxelNode(voxels, depth, local);
    zox_tst_check(!voxel, "character is inside a solid block [%i] at [%ix%ix%i]", voxel, block.x, block.y, block.z);
    // standing on ground, not floating or mid fall: a solid block close under it
    const int ground_range = 4;
    int ground = 0;
    for (int i = 1; i <= ground_range && !ground; i++) {
        const int under = zox_tst_terrain_voxel(world, terrain, (int3) { block.x, block.y - i, block.z });
        if (under > 0) {
            ground = i;
        }
    }
    zox_tst_check(ground, "character isn't standing on ground, no solid block within [%i] under [%ix%ix%i]",
        ground_range, block.x, block.y, block.z);
}

// tst: continue_game, continue -> load the newest save -> confirm, the player spawns in the terrain
//  - reuses load_games (its saves have no player.dat, so it places the player like a new game)
//  - then the same checks as player_spawn
byte zox_tst_continue_loaded = 0;
byte zox_tst_continue_confirmed = 0;

byte zox_tst_continue_game_ready(ecs* world) {
    if (!zox_tst_continue_loaded) {
        zox_tst_continue_loaded = zox_tst_load_games_ready(world);
        return 0;
    }
    if (!zox_tst_continue_confirmed) {
        zox_tst_waiting = "the realm menu";
        const entity player = dbg_player;
        const entity canvas = zox_get_link(world, player, CanvasLink);
        const entity menu = zox_get_child_by_id(world, canvas, zox_id(MenuRealm));
        if (!zox_valid(menu)) {
            return 0;
        }
        const entity button = zox_tst_find_click(world, menu, &button_event_menu_realm_confirm);
        zox_tst_check(button, "realm menu has no confirm button");
        if (!button) {
            return 1;
        }
        zox_tst_click(world, player, button);
        zox_tst_continue_confirmed = 1;
        zox_tst_spawn_start = zox_current_time;
        return 0;
    }
    return zox_tst_player_spawn_ready(world);
}

// tst: stream_npcs, the player walks across chunks, npcs spawn and despawn with their chunks
//  - after the player spawns, moves its character one chunk at a time around a loop
//  - after each step settles, checks every chunk and npc:
//      - a chunk's linked npcs are alive, and no npc is linked from two chunks
//      - every npc in the realm belongs to a chunk (no orphans left by despawning)
//      - a chunk whose spawn zone settled inactive has no npcs left
//      - npcs of chunks that stayed active survive the step (walking used to despawn + respawn them)
//      - a deleted npc's 3D uis (ElementLinks) are deleted too
//  - by the end, npcs were both spawned and despawned (by going inactive, or by their chunk unloading)
//  - a crash while streaming fails it (bash bsh/tst.sh stream_npcs --gdb for a backtrace)
static const sbyte2 zox_tst_stream_path[] = {
    { 1, 0 }, { 1, 0 }, { 1, 0 }, { 1, 0 },
    { 0, 1 }, { 0, 1 },
    { -1, 0 }, { -1, 0 }, { -1, 0 }, { -1, 0 },
    { 0, -1 }, { 0, -1 },
};
#define zox_tst_stream_steps (int) (sizeof(zox_tst_stream_path) / sizeof(sbyte2))
const double zox_tst_stream_step_seconds = 3.0;
int zox_tst_stream_step = -1;   // -1 waiting for the player to spawn
double zox_tst_stream_step_time = 0;
int zox_tst_stream_most_npcs = 0;
int zox_tst_stream_despawned_chunks = 0;    // chunks seen going inactive after having npcs
int zox_tst_stream_failed_steps = 0;
// last verify's npcs (with their chunk) and their 3D uis, to check what changed after a step
typedef struct {
    entity chunk;
    entity npc;
} zox_tst_stream_pair;
#define zox_tst_stream_max_pairs 8192
zox_tst_stream_pair zox_tst_stream_npcs_before[zox_tst_stream_max_pairs];
int zox_tst_stream_npcs_before_count = 0;
zox_tst_stream_pair zox_tst_stream_uis_before[zox_tst_stream_max_pairs];  // chunk field holds the npc's ui element
int zox_tst_stream_uis_before_count = 0;
#define zox_tst_stream_max_chunks 4096
entity zox_tst_stream_had_npcs[zox_tst_stream_max_chunks];
int zox_tst_stream_had_npcs_count = 0;

static inline byte zox_tst_stream_had_npcs_has(entity chunk) {
    for (int i = 0; i < zox_tst_stream_had_npcs_count; i++) {
        if (zox_tst_stream_had_npcs[i] == chunk) {
            return 1;
        }
    }
    return 0;
}

void zox_tst_stream_verify(ecs* world, const char* label) {
    const entity terrain = local_terrain;
    const entity realm = zox_valid(terrain) ? zox_get_parent(world, terrain) : 0;
    if (!zox_valid(realm)) {
        zox_tst_check(0, "[%s] no terrain or realm", label);
        return;
    }
    // npcs linked from chunks
    static entity linked[8192];
    int linked_count = 0;
    int chunks = 0;
    int active_chunks = 0;
    int dead_links = 0;
    int double_links = 0;
    int inactive_with_npcs = 0;
    char first_error[256] = { 0 };
    iter it2 = zox_children(world, terrain);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            const entity chunk = it2.entities[j];
            if (!zox_valid(chunk) || !zox_has(chunk, NpcSpawnZone)) {
                continue;
            }
            chunks++;
            const byte active = zox_getv(chunk, NpcSpawnZone);
            const byte dirty = zox_getv(chunk, NpcSpawnZoneDirty);
            active_chunks += active;
            int npcs = 0;
            zox_each_link(world, chunk, ChunkCharacters, npc) {
                npcs++;
                if (!zox_valid(npc)) {
                    if (!dead_links++) snprintf(first_error, sizeof(first_error), "chunk [%s] links a dead npc", zox_getn(chunk));
                    continue;
                }
                for (int k = 0; k < linked_count; k++) {
                    if (linked[k] == npc) {
                        if (!double_links++) snprintf(first_error, sizeof(first_error), "npc [%s] linked from two chunks", zox_getn(npc));
                    }
                }
                if (linked_count < 8192) {
                    linked[linked_count++] = npc;
                }
            }
            if (npcs && !zox_tst_stream_had_npcs_has(chunk) && zox_tst_stream_had_npcs_count < zox_tst_stream_max_chunks) {
                zox_tst_stream_had_npcs[zox_tst_stream_had_npcs_count++] = chunk;
            }
            if (!active && !dirty) {
                if (npcs) {
                    if (!inactive_with_npcs++) snprintf(first_error, sizeof(first_error), "inactive chunk [%s] still has [%i] npcs", zox_getn(chunk), npcs);
                } else if (zox_tst_stream_had_npcs_has(chunk)) {
                    zox_tst_stream_despawned_chunks++;
                    // count each despawn once
                    for (int k = 0; k < zox_tst_stream_had_npcs_count; k++) {
                        if (zox_tst_stream_had_npcs[k] == chunk) {
                            zox_tst_stream_had_npcs[k] = zox_tst_stream_had_npcs[--zox_tst_stream_had_npcs_count];
                            break;
                        }
                    }
                }
            }
        }
    }
    // chunks that had npcs and were unloaded also despawned them (orphans below checks they're gone)
    //  - npcs stay active one chunk past their spawn range, which can be past the stream range
    for (int k = zox_tst_stream_had_npcs_count - 1; k >= 0; k--) {
        if (!zox_valid(zox_tst_stream_had_npcs[k])) {
            zox_tst_stream_despawned_chunks++;
            zox_tst_stream_had_npcs[k] = zox_tst_stream_had_npcs[--zox_tst_stream_had_npcs_count];
        }
    }
    // npcs in the realm that no chunk owns
    int npcs_total = 0;
    int orphans = 0;
    iter it3 = zox_children(world, realm);
    while (zox_children_next(it3)) {
        for (int j = 0; j < it3.count; j++) {
            const entity npc = it3.entities[j];
            // NOTE: the realm's character metas are prefab npcs, they aren't spawned by chunks
            if (!zox_valid(npc) || !zox_has(npc, Npc) || zox_is_prefab(npc)) {
                continue;
            }
            npcs_total++;
            byte owned = 0;
            for (int k = 0; k < linked_count; k++) {
                if (linked[k] == npc) {
                    owned = 1;
                    break;
                }
            }
            if (!owned && !orphans++) {
                snprintf(first_error, sizeof(first_error), "npc [%s] isn't owned by any chunk", zox_getn(npc));
            }
        }
    }
    if (npcs_total > zox_tst_stream_most_npcs) {
        zox_tst_stream_most_npcs = npcs_total;
    }
    // npcs of chunks that stayed active must survive the step (no despawn + respawn when walking)
    int flickered = 0;
    for (int k = 0; k < zox_tst_stream_npcs_before_count; k++) {
        const zox_tst_stream_pair pair = zox_tst_stream_npcs_before[k];
        if (zox_valid(pair.npc) || !zox_valid(pair.chunk)) {
            continue;
        }
        if (zox_getv(pair.chunk, NpcSpawnZone) && !zox_getv(pair.chunk, NpcSpawnZoneDirty)) {
            if (!flickered++) snprintf(first_error, sizeof(first_error),
                "chunk [%s] stayed active but its npc was deleted (despawned and respawned)", zox_getn(pair.chunk));
        }
    }
    // a deleted npc's 3D uis go with it
    int ui_orphans = 0;
    for (int k = 0; k < zox_tst_stream_uis_before_count; k++) {
        const entity npc = zox_tst_stream_uis_before[k].npc;
        const entity ui = zox_tst_stream_uis_before[k].chunk;
        if (!zox_valid(npc) && zox_valid(ui)) {
            if (!ui_orphans++) snprintf(first_error, sizeof(first_error),
                "ui [%s] outlived its deleted npc", zox_getn(ui));
        }
    }
    // remember this verify's npcs and uis for the next one
    zox_tst_stream_npcs_before_count = 0;
    zox_tst_stream_uis_before_count = 0;
    iter it4 = zox_children(world, terrain);
    while (zox_children_next(it4)) {
        for (int j = 0; j < it4.count; j++) {
            const entity chunk = it4.entities[j];
            if (!zox_valid(chunk) || !zox_has(chunk, NpcSpawnZone)) {
                continue;
            }
            const byte settled_active = zox_getv(chunk, NpcSpawnZone) && !zox_getv(chunk, NpcSpawnZoneDirty);
            zox_each_link(world, chunk, ChunkCharacters, npc) {
                if (!zox_valid(npc)) {
                    continue;
                }
                if (settled_active && zox_tst_stream_npcs_before_count < zox_tst_stream_max_pairs) {
                    zox_tst_stream_npcs_before[zox_tst_stream_npcs_before_count++] = (zox_tst_stream_pair) { chunk, npc };
                }
                zox_each_link(world, npc, ElementLinks, ui) {
                    if (zox_valid(ui) && zox_tst_stream_uis_before_count < zox_tst_stream_max_pairs) {
                        zox_tst_stream_uis_before[zox_tst_stream_uis_before_count++] = (zox_tst_stream_pair) { ui, npc };
                    }
                }
            }
        }
    }
    const int errors = dead_links + double_links + inactive_with_npcs + orphans + flickered + ui_orphans;
    zox_log("[tst] [%s] chunks [%i] active [%i] npcs [%i] linked [%i] errors: dead [%i] double [%i] inactive [%i] orphans [%i] flickered [%i] ui orphans [%i]",
        label, chunks, active_chunks, npcs_total, linked_count, dead_links, double_links, inactive_with_npcs, orphans, flickered, ui_orphans);
    zox_tst_check(!errors, "[%s] [%i] npc errors, first: %s", label, errors, first_error);
    if (errors) {
        zox_tst_stream_failed_steps++;
    }
}

byte zox_tst_stream_npcs_ready(ecs* world) {
    if (zox_tst_stream_step == -1) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        zox_tst_stream_step = 0;
        zox_tst_stream_step_time = zox_current_time;
        zox_tst_stream_verify(world, "spawned");
        return 0;
    }
    zox_tst_waiting = "the player to walk the chunk loop";
    zox_tst_progress(zox_tst_stream_step);
    if (zox_current_time - zox_tst_stream_step_time < zox_tst_stream_step_seconds) {
        return 0;
    }
    // unlink callbacks run next frame (i.e. a deleted chunk's npcs), check once they've run
    if (zox_unlinks_count) {
        return 0;
    }
    if (zox_tst_stream_step > 0) {
        char label[32];
        snprintf(label, sizeof(label), "step %i", zox_tst_stream_step);
        zox_tst_stream_verify(world, label);
        if (zox_tst_stream_failed_steps >= 3) {
            return 1;
        }
    }
    if (zox_tst_stream_step >= zox_tst_stream_steps) {
        return 1;
    }
    const entity character = zox_tst_spawn_character(world);
    zox_tst_check(character, "player character is gone at step [%i]", zox_tst_stream_step);
    if (!character) {
        return 1;
    }
    // move one chunk, a little up so it lands on the new ground
    const entity terrain = local_terrain;
    const float scale = zox_getv(terrain, BlockScale);
    const float chunk_length = octree_size(zox_getv(terrain, NodeDepth)) * scale;
    const sbyte2 direction = zox_tst_stream_path[zox_tst_stream_step];
    float3 position = zox_getv(character, Position3D);
    position.x += direction.x * chunk_length;
    position.z += direction.y * chunk_length;
    position.y += scale * 2;
    zox_setv(character, Position3D, position);
    zox_tst_stream_step++;
    zox_tst_stream_step_time = zox_current_time;
    return 0;
}

void zox_tst_stream_npcs_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_smooth_lights_errors == 0, "[%u] Smooth Lights Colors Past Limits errors while streaming", zox_smooth_lights_errors);
    zox_log("[tst] most npcs at once [%i], chunks that despawned their npcs [%i]",
        zox_tst_stream_most_npcs, zox_tst_stream_despawned_chunks);
    zox_tst_check(zox_tst_stream_step >= zox_tst_stream_steps, "only walked [%i] of [%i] steps", zox_tst_stream_step, zox_tst_stream_steps);
    zox_tst_check(zox_tst_stream_most_npcs > 0, "no npcs ever spawned");
    zox_tst_check(zox_tst_stream_despawned_chunks > 0, "no chunk ever despawned its npcs while walking");
}

// tst: screenshot, saves what the player sees after spawning to tst/screenshot.ppm
//  - for looking at the rendering (i.e. lighting), bash bsh/tst.sh screenshot, then convert to png
//  - waits zox_tst_screenshot_wait after the player settles, so lights and meshes finish
//  - zox_tst_save=<realm folder> loads a copy of a real save instead (same place every run, for before / after)
extern char zox_screenshot_request[256];
const double zox_tst_screenshot_wait = 6.0;
byte zox_tst_screenshot_stage = 0;
double zox_tst_screenshot_time = 0;

byte zox_tst_screenshot_ready(ecs* world) {
    if (zox_tst_screenshot_stage == 0) {
        const char* real_save = getenv("zox_tst_save");
        const byte spawned = real_save && real_save[0] ?
            zox_tst_continue_game_ready(world) :
            zox_tst_player_spawn_ready(world);
        if (!spawned) {
            return 0;
        }
        zox_tst_screenshot_stage = 1;
        zox_tst_screenshot_time = zox_current_time;
    }
    zox_tst_waiting = "lights and meshes to finish before the screenshot";
    zox_tst_progress(zox_current_time - zox_tst_screenshot_time);
    if (zox_tst_screenshot_stage == 1) {
        if (zox_current_time - zox_tst_screenshot_time < zox_tst_screenshot_wait) {
            return 0;
        }
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("screenshot.ppm"));
        zox_tst_screenshot_stage = 2;
        return 0;
    }
    // saved at the end of the frame it was requested in
    return !zox_screenshot_request[0];
}

void zox_tst_screenshot_check(ecs* world) {
    (void) world;
    FILE* file = fopen(zox_tst_path("screenshot.ppm"), "rb");
    zox_tst_check(file, "no screenshot saved");
    if (file) {
        fclose(file);
    }
}

#include "perf.c"
#include "body_views.c"

void zox_tsts_controllers(ecs* world) {
    zox_tst_add("screenshot", zox_tst_screenshot_ready, zox_tst_screenshot_check, 90);
    zox_tst_add("body_views", zox_tst_body_views_ready, zox_tst_body_views_check, 120);
    zox_tst_set_setup("body_views", NULL, 1);
    const char* real_save = getenv("zox_tst_save");
    if (real_save && real_save[0]) {
        zox_tst_set_setup("screenshot", zox_tst_load_games_setup, 1);
    }
    (void) world;
    zox_tst_add("player_spawn", zox_tst_player_spawn_ready, zox_tst_player_spawn_check, 60);
    zox_tst_add("continue_game", zox_tst_continue_game_ready, zox_tst_player_spawn_check, 90);
    zox_tst_set_setup("continue_game", zox_tst_load_games_setup, 1);
    zox_tst_add("links_cleanup", zox_tst_links_cleanup_ready, zox_tst_links_cleanup_check, 30);
    zox_tst_add("stream_npcs", zox_tst_stream_npcs_ready, zox_tst_stream_npcs_check, 95);
    zox_tst_add("action_wheel", zox_tst_action_wheel_ready, zox_tst_action_wheel_check, 60);
    zox_tst_add("item_split", zox_tst_item_split_ready, zox_tst_item_split_check, 90);
    zox_tst_add("resume_no_punch", zox_tst_resume_no_punch_ready, zox_tst_resume_no_punch_check, 90);
    zox_tst_add("pickup_pop", zox_tst_pickup_pop_ready, zox_tst_pickup_pop_check, 90);
    zox_tst_add("quest_tracking", zox_tst_quest_tracking_ready, zox_tst_quest_tracking_check, 90);
    zox_tst_add("quest_dialogue", zox_tst_quest_dialogue_ready, zox_tst_quest_dialogue_check, 120);
    zox_tst_add("npc_stats", zox_tst_npc_stats_ready, zox_tst_npc_stats_check, 120);
    zox_tst_add("hit_sound_links", zox_tst_hit_sound_links_ready, zox_tst_hit_sound_links_check, 120);
    zox_tst_add("clothes", zox_tst_clothes_ready, zox_tst_clothes_check, 120);
    zox_tst_add("npc_attacks", zox_tst_npc_attacks_ready, zox_tst_npc_attacks_check, 120);
    zox_tst_add("region_population", zox_tst_region_population_ready, zox_tst_region_population_check, 120);
    zox_tst_add("npc_leash", zox_tst_npc_leash_ready, zox_tst_npc_leash_check, 120);
    zox_tst_add("player_respawn", zox_tst_player_respawn_ready, zox_tst_player_respawn_check, 120);
    zox_tst_add("footsteps", zox_tst_footsteps_ready, zox_tst_footsteps_check, 120);
    zox_tst_add("arms_in_step", zox_tst_arms_in_step_ready, zox_tst_arms_in_step_check, 120);
    zox_tst_add("lod_chunk_lights", zox_tst_lod_chunk_lights_ready, zox_tst_lod_chunk_lights_check, 120);
    zox_tst_add("npc_skeletons", zox_tst_npc_skeletons_ready, zox_tst_npc_skeletons_check, 120);
    zox_tst_add("npc_glance", zox_tst_npc_glance_ready, zox_tst_npc_glance_check, 120);
    zox_tst_add("perf_stream", zox_tst_perf_stream_ready, zox_tst_perf_stream_check, 180);
    zox_tst_add("perf_fly", zox_tst_perf_fly_ready, zox_tst_perf_fly_check, 120);
    zox_tst_add("tables_stream", zox_tst_perf_stream_ready, zox_tst_tables_stream_check, 200);
    zox_tst_add("pipeline_syncs", zox_tst_player_spawn_ready, zox_tst_pipeline_syncs_check, 120);
    zox_tst_add("lines_render", zox_tst_lines_render_ready, zox_tst_lines_render_check, 120);
    zox_tst_add("town_maps", zox_tst_town_maps_ready, zox_tst_town_maps_check, 120);
    zox_tst_add("town_maps_empty", zox_tst_town_maps_empty_ready, zox_tst_town_maps_empty_check, 120);
    zox_tst_add("region_zones", zox_tst_region_zones_ready, zox_tst_region_zones_check, 120);
    zox_tst_add("home_roofs", zox_tst_region_zones_ready, zox_tst_home_roofs_check, 120);
    zox_tst_add("town_homes", zox_tst_town_homes_ready, zox_tst_town_homes_check, 120);
    zox_tst_add("home_glow", zox_tst_home_glow_ready, zox_tst_home_glow_check, 120);
    zox_tst_add("glow_light", zox_tst_glow_light_ready, zox_tst_glow_light_check, 120);
    zox_tst_add("block_table", zox_tst_town_homes_ready, zox_tst_block_table_check, 120);
    zox_tst_add("sky_clouds", zox_tst_sky_clouds_ready, zox_tst_sky_clouds_check, 120);
    zox_tst_add("head_camera", zox_tst_head_camera_ready, zox_tst_head_camera_check, 120);
    zox_tst_add("vegetation_chunks", zox_tst_vegetation_chunks_ready, zox_tst_vegetation_chunks_check, 120);
    zox_tst_add("music_crossfade", zox_tst_music_crossfade_ready, zox_tst_music_crossfade_check, 120);
    zox_tst_set_setup("music_crossfade", NULL, 1);
}
