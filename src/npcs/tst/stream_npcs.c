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
    byte died;      // seen dying (Dead, PreDeath): a death isn't a despawn (villagers fight slimes, villager_guard)
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
        if (zox_valid(pair.npc) || !zox_valid(pair.chunk) || pair.died) {
            continue;
        }
        if (zox_getv(pair.chunk, NpcSpawnZone) && !zox_getv(pair.chunk, NpcSpawnZoneDirty)) {
            if (!flickered++) snprintf(first_error, sizeof(first_error),
                "chunk [%s] stayed active but its npc was deleted (despawned and respawned)", zox_getn(pair.chunk));
        }
    }
    int died = 0;
    for (int k = 0; k < zox_tst_stream_npcs_before_count; k++) {
        died += zox_tst_stream_npcs_before[k].died;
    }
    if (died) {
        zox_log("[tst] [%s] [%i] npcs died (fights), not despawns", label, died);
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
                    zox_tst_stream_npcs_before[zox_tst_stream_npcs_before_count++] = (zox_tst_stream_pair) { chunk, npc, 0 };
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

// the npcs seen dying since the last verify, each frame (their deaths aren't despawns)
static inline void zox_tst_stream_mark_deaths(ecs* world) {
    for (int k = 0; k < zox_tst_stream_npcs_before_count; k++) {
        zox_tst_stream_pair* pair = &zox_tst_stream_npcs_before[k];
        if (!pair->died && zox_valid(pair->npc) && (zox_has(pair->npc, Dead) || zox_has(pair->npc, PreDeath))) {
            pair->died = 1;
        }
    }
}

byte zox_tst_stream_npcs_ready(ecs* world) {
    zox_tst_stream_mark_deaths(world);
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
    const entity character = zox_tst_player_character(world);
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
    zox_log("[tst] [metric] stale meshes rebuilt when lit while streaming [%u]", zox_smooth_lights_stale);
    zox_log("[tst] most npcs at once [%i], chunks that despawned their npcs [%i]",
        zox_tst_stream_most_npcs, zox_tst_stream_despawned_chunks);
    zox_tst_check(zox_tst_stream_step >= zox_tst_stream_steps, "only walked [%i] of [%i] steps", zox_tst_stream_step, zox_tst_stream_steps);
    zox_tst_check(zox_tst_stream_most_npcs > 0, "no npcs ever spawned");
    zox_tst_check(zox_tst_stream_despawned_chunks > 0, "no chunk ever despawned its npcs while walking");
}

void zox_tsts_stream_npcs(ecs* world) {
    zox_tst_add("stream_npcs", zox_tst_stream_npcs_ready, zox_tst_stream_npcs_check, 95);
}
