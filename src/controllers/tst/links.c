// tst: links_cleanup, deleting a link's target removes the link, and owned links delete their holder
//  - the real relations, set up like npcs: a chunk links its npcs (ChunkCharacters, nf_links + unlink hook),
//    an npc links its uis (ElementLinks), a ui is owned by its npc (UIHolderLink, deleted with its target)
//  - every scenario deletes npcs a different way the game does, many times over
//  - after each: the chunk links only alive npcs, and every deleted npc's uis are gone
//  - run: bash bsh/tst.sh links_cleanup
#define zox_tst_links_npcs 8
#define zox_tst_links_rounds 50

typedef struct {
    entity chunk;
    entity realm;
    entity npcs[zox_tst_links_npcs];
    entity uis[zox_tst_links_npcs];
} zox_tst_links_set;

entity zox_tst_links_prefab = 0;

static inline zox_tst_links_set zox_tst_links_build(ecs* world, byte use_prefab, byte use_parent) {
    zox_tst_links_set set = { 0 };
    set.chunk = zox_new();
    set.realm = zox_new();
    for (int i = 0; i < zox_tst_links_npcs; i++) {
        entity npc = use_prefab ? zox_ins(world, zox_tst_links_prefab) : zox_new();
        zox_add(npc, Npc);
        if (use_parent) {
            zox_set_parent(world, npc, set.realm);
        }
        entity ui = zox_new();
        zox_link(world, ui, UIHolderLink, npc);
        zox_link(world, npc, ElementLinks, ui);
        zox_link(world, set.chunk, ChunkCharacters, npc);
        set.npcs[i] = npc;
        set.uis[i] = ui;
    }
    return set;
}

// returns errors: links to dead npcs, missing links to alive npcs, uis that outlived their npc
static inline int zox_tst_links_verify(ecs* world, const zox_tst_links_set* set, char* first, int first_size) {
    int errors = 0;
    int alive = 0;
    for (int i = 0; i < zox_tst_links_npcs; i++) {
        const entity npc = set->npcs[i];
        if (zox_valid(npc)) {
            alive++;
            if (!zox_is_linked(world, set->chunk, ChunkCharacters, npc)) {
                if (!errors++) snprintf(first, first_size, "alive npc [%i] lost its chunk link", i);
            }
        } else if (zox_valid(set->uis[i])) {
            const entity holder = zox_get_link(world, set->uis[i], UIHolderLink);
            if (!errors++) snprintf(first, first_size, "ui of deleted npc [%i] survived, its holder link [%llu] npc was [%llu]",
                i, (unsigned long long) holder, (unsigned long long) set->npcs[i]);
        }
    }
    int linked = 0;
    zox_each_link(world, set->chunk, ChunkCharacters, npc) {
        linked++;
        if (!zox_valid(npc)) {
            if (!errors++) snprintf(first, first_size, "chunk still links a deleted npc");
        }
    }
    if (linked != alive) {
        if (!errors++) snprintf(first, first_size, "chunk links [%i] npcs, [%i] alive", linked, alive);
    }
    // which cleanup failed: chunk links (unlink hook) vs owned uis (OnDeleteTarget Delete)
    int stale_chunk_links = linked - alive;
    int stale_uis = 0;
    for (int i = 0; i < zox_tst_links_npcs; i++) {
        stale_uis += !zox_valid(set->npcs[i]) && zox_valid(set->uis[i]);
    }
    if (errors) {
        char detail[64];
        snprintf(detail, sizeof(detail), " (stale chunk links [%i] uis [%i])", stale_chunk_links, stale_uis);
        strncat(first, detail, first_size - strlen(first) - 1);
    }
    return errors;
}

static inline void zox_tst_links_free(ecs* world, zox_tst_links_set* set) {
    for (int i = 0; i < zox_tst_links_npcs; i++) {
        zox_delete_safe(set->npcs[i]);
        zox_delete_safe(set->uis[i]);
    }
    zox_delete_safe(set->chunk);
    zox_delete_safe(set->realm);
}

// how a scenario deletes npcs, odd npcs only so links to alive ones are checked too
typedef void (*zox_tst_links_scenario)(ecs* world, zox_tst_links_set* set, int round);

void zox_tst_links_immediate(ecs* world, zox_tst_links_set* set, int round) {
    (void) round;
    for (int i = 1; i < zox_tst_links_npcs; i += 2) {
        zox_delete(set->npcs[i]);
    }
}

void zox_tst_links_deferred(ecs* world, zox_tst_links_set* set, int round) {
    (void) round;
    zox_defer_begin();
    for (int i = 1; i < zox_tst_links_npcs; i += 2) {
        zox_delete(set->npcs[i]);
    }
    zox_defer_end();
}

// TerrainCharactersDespawnSystem: deletes all while iterating the links, then changes the chunk
void zox_tst_links_despawn_all(ecs* world, zox_tst_links_set* set, int round) {
    (void) round;
    zox_defer_begin();
    zox_each_link(world, set->chunk, ChunkCharacters, npc) {
        zox_delete(npc);
    }
    zox_add(set->chunk, CharactersSpawned);
    zox_remove(set->chunk, CharactersSpawned);
    zox_defer_end();
}

// npcs changed in the same batch they're deleted in (moved chunk, relinked ui, new tags)
void zox_tst_links_busy_tag(ecs* world, zox_tst_links_set* set, int round) {
    (void) round;
    zox_defer_begin();
    for (int i = 1; i < zox_tst_links_npcs; i += 2) {
        zox_add(set->npcs[i], DisableMovement);
        zox_delete(set->npcs[i]);
    }
    zox_defer_end();
}

void zox_tst_links_busy_new_ui(ecs* world, zox_tst_links_set* set, int round) {
    (void) round;
    zox_defer_begin();
    for (int i = 1; i < zox_tst_links_npcs; i += 2) {
        const entity npc = set->npcs[i];
        entity ui = zox_new();
        zox_link(world, ui, UIHolderLink, npc);
        zox_link(world, npc, ElementLinks, ui);
        zox_delete(npc);
    }
    zox_defer_end();
}

void zox_tst_links_busy_relink(ecs* world, zox_tst_links_set* set, int round) {
    (void) round;
    zox_defer_begin();
    for (int i = 1; i < zox_tst_links_npcs; i += 2) {
        const entity npc = set->npcs[i];
        zox_unlink(world, set->chunk, ChunkCharacters, npc);
        zox_link(world, set->chunk, ChunkCharacters, npc);
        zox_delete(npc);
    }
    zox_defer_end();
}

// relinked in one batch, deleted in the next
void zox_tst_links_relink_then_delete(ecs* world, zox_tst_links_set* set, int round) {
    (void) round;
    {
        zox_defer_begin();
        for (int i = 1; i < zox_tst_links_npcs; i += 2) {
            zox_unlink(world, set->chunk, ChunkCharacters, set->npcs[i]);
            zox_link(world, set->chunk, ChunkCharacters, set->npcs[i]);
        }
        zox_defer_end();
    }
    zox_tst_links_deferred(world, set, round);
}

// unlinked in one batch, deleted in the next (no relink)
void zox_tst_links_unlink_then_delete(ecs* world, zox_tst_links_set* set, int round) {
    (void) round;
    {
        zox_defer_begin();
        for (int i = 1; i < zox_tst_links_npcs; i += 2) {
            zox_unlink(world, set->chunk, ChunkCharacters, set->npcs[i]);
        }
        zox_defer_end();
    }
    zox_tst_links_deferred(world, set, round);
}

// deleted twice over: by the despawn and by its chunk going away in the same batch
void zox_tst_links_with_chunk(ecs* world, zox_tst_links_set* set, int round) {
    (void) round;
    zox_defer_begin();
    zox_each_link(world, set->chunk, ChunkCharacters, npc) {
        zox_delete(npc);
    }
    zox_delete(set->chunk);
    zox_defer_end();
    set->chunk = 0;
}

// the parent (realm) is deleted, npcs go by cascade
void zox_tst_links_cascade(ecs* world, zox_tst_links_set* set, int round) {
    (void) round;
    zox_defer_begin();
    zox_delete(set->realm);
    zox_defer_end();
    set->realm = 0;
}

typedef struct {
    const char* name;
    zox_tst_links_scenario run;
    byte use_prefab;
    byte use_parent;
    byte chunk_deleted;
} zox_tst_links_case;

void zox_tst_links_cleanup_check(ecs* world) {
    if (!zox_tst_links_prefab) {
        zox_tst_links_prefab = zox_new();
        zox_add(zox_tst_links_prefab, Npc);
        zox_make_prefab(zox_tst_links_prefab);
    }
    const zox_tst_links_case cases[] = {
        { "immediate", zox_tst_links_immediate, 0, 0, 0 },
        { "deferred", zox_tst_links_deferred, 0, 0, 0 },
        { "deferred prefab", zox_tst_links_deferred, 1, 1, 0 },
        { "despawn all", zox_tst_links_despawn_all, 1, 1, 0 },
        { "with chunk", zox_tst_links_with_chunk, 1, 1, 1 },
        { "cascade", zox_tst_links_cascade, 1, 1, 0 },
        { "busy tag", zox_tst_links_busy_tag, 1, 1, 0 },
        { "busy new ui", zox_tst_links_busy_new_ui, 1, 1, 0 },
        { "busy relink", zox_tst_links_busy_relink, 1, 1, 0 },
        { "relink then delete", zox_tst_links_relink_then_delete, 1, 1, 0 },
        { "unlink then delete", zox_tst_links_unlink_then_delete, 1, 1, 0 },
        { "deferred after all", zox_tst_links_deferred, 1, 1, 0 },
    };
    // ZOX_TST_LINKS_CASE=<name> runs one case, so others can't leave flecs in a bad state first
    const char* only = getenv("ZOX_TST_LINKS_CASE");
    for (int c = 0; c < (int) (sizeof(cases) / sizeof(cases[0])); c++) {
        if (only && strcmp(only, cases[c].name) != 0) {
            continue;
        }
        int failed_rounds = 0;
        char first[260] = { 0 };
        for (int round = 0; round < zox_tst_links_rounds; round++) {
            zox_tst_links_set set = zox_tst_links_build(world, cases[c].use_prefab, cases[c].use_parent);
            cases[c].run(world, &set, round);
            char error[200] = { 0 };
            int errors = 0;
            if (cases[c].chunk_deleted) {
                // no chunk to check, every npc and ui must be gone
                for (int i = 0; i < zox_tst_links_npcs; i++) {
                    if (zox_valid(set.npcs[i]) || zox_valid(set.uis[i])) {
                        if (!errors++) snprintf(error, sizeof(error), "npc or ui [%i] survived", i);
                    }
                }
            } else {
                errors = zox_tst_links_verify(world, &set, error, sizeof(error));
            }
            if (errors && !failed_rounds++) {
                snprintf(first, sizeof(first), "round [%i]: %s", round, error);
            }
            zox_tst_links_free(world, &set);
        }
        zox_log("[tst] links [%s] failed [%i/%i] rounds", cases[c].name, failed_rounds, zox_tst_links_rounds);
        zox_tst_check(!failed_rounds, "[%s] failed [%i/%i] rounds, first %s",
            cases[c].name, failed_rounds, zox_tst_links_rounds, first);
    }
}

byte zox_tst_links_cleanup_ready(ecs* world) {
    (void) world;
    return 1;
}
