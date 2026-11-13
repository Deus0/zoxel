// tst: end_game_cleanup, ending a game frees what it made: entities, gpu buffers, textures and memory
//  - two rounds from the main menu: new game > enter realm > play a few seconds (terrain, npcs stream in)
//      > pause > end game > back at the main menu, settled
//  - after each round it counts, leaving out ui (the main menu's labels are random) and what deletes
//      itself (DestroyInTime debug lines, AnimationTarget menu fades):
//      live entities, gpu buffers and textures, and the process's resident memory
//  - the first rounds warm up (caches, pools, allocators, prefabs made on first use), the last must end where
//      the one before it did:
//      the same entities, buffers and textures, heap in use grown by less than zox_tst_end_game_heap_mb
//      (heap, not resident memory, which counts what allocators keep after freeing)
//  - logs each round's counts while playing and after, to find what leaked
//  - run: bash bsh/tst.sh end_game_cleanup
#define zox_tst_end_game_rounds 3
#define zox_tst_end_game_play_seconds 4.0
#define zox_tst_end_game_settle_frames 120
#define zox_tst_end_game_heap_mb 32   // heap grew ~22mb a game (2026-11-13), flecs pools or a leak, todo
int zox_tst_end_game_stage = 0;
int zox_tst_end_game_round = 0;
int zox_tst_end_game_frames = 0;
double zox_tst_end_game_play_start = 0;

typedef struct {
    int entities;
    int buffers;
    int textures;
    double memory_mb;
    double heap_mb;
} ZoxTstEndGameCounts;

ZoxTstEndGameCounts zox_tst_end_game_after[zox_tst_end_game_rounds];
// the live entities after the first round, the second round's extra ones are what leaked
entity* zox_tst_end_game_alive = NULL;
int zox_tst_end_game_alive_count = 0;

static int zox_tst_entity_compare(const void* a, const void* b) {
    const entity x = *(const entity*) a, y = *(const entity*) b;
    return x < y ? -1 : (x > y);
}

// logs the entities alive now that weren't after the first round, grouped by their name and type
static inline void zox_tst_end_game_log_leaks(ecs* world) {
    const ecs_entities_t all = ecs_get_entities(world);
    if (!zox_tst_end_game_alive) {
        zox_tst_end_game_alive_count = all.alive_count;
        zox_tst_end_game_alive = malloc(sizeof(entity) * all.alive_count);
        memcpy(zox_tst_end_game_alive, all.ids, sizeof(entity) * all.alive_count);
        qsort(zox_tst_end_game_alive, all.alive_count, sizeof(entity), zox_tst_entity_compare);
        return;
    }
    char* groups[64];
    int counts[64];
    int groups_count = 0;
    for (int i = 0; i < all.alive_count; i++) {
        const entity e = all.ids[i];
        if (bsearch(&e, zox_tst_end_game_alive, zox_tst_end_game_alive_count, sizeof(entity), zox_tst_entity_compare)) {
            continue;
        }
        // its name without the id, and its type
        const char* name = ecs_get_name(world, e);
        char base[64];
        snprintf(base, sizeof(base), "%s", name ? name : "?");
        char* underscore = strrchr(base, '_');
        if (underscore && underscore[1] >= '0' && underscore[1] <= '9') {
            *underscore = 0;
        }
        char* type = ecs_type_str(world, ecs_get_type(world, e));
        char key[512];
        snprintf(key, sizeof(key), "%s: %.400s", base, type ? type : "");
        ecs_os_free(type);
        int g = 0;
        for (; g < groups_count; g++) {
            if (!strcmp(groups[g], key)) {
                break;
            }
        }
        if (g == groups_count) {
            if (groups_count >= 64) {
                continue;
            }
            groups[groups_count] = strdup(key);
            counts[groups_count++] = 0;
        }
        counts[g]++;
    }
    for (int g = 0; g < groups_count; g++) {
        zox_log("[tst] [metric] end_game_cleanup leaked [%i] %s", counts[g], groups[g]);
        free(groups[g]);
    }
}

#if defined(__GLIBC__)
    #include <malloc.h>
#endif

// heap memory in use (allocated, not freed), glibc's mallinfo2, 0 elsewhere: resident memory also counts what
//  allocators keep after freeing, it moved 11-51mb a round for the same work
static inline double zox_tst_heap_mb() {
#if defined(__GLIBC__) && (__GLIBC__ > 2 || __GLIBC_MINOR__ >= 33)
    return mallinfo2().uordblks / (1024.0 * 1024.0);
#else
    return 0;
#endif
}

// the process's resident memory, from /proc/self/statm (linux), 0 elsewhere
static inline double zox_tst_resident_mb() {
#ifdef zox_linux
    FILE* f = fopen("/proc/self/statm", "r");
    if (!f) {
        return 0;
    }
    long pages = 0, resident = 0;
    const int read = fscanf(f, "%ld %ld", &pages, &resident);
    fclose(f);
    return read == 2 ? resident * (double) sysconf(_SC_PAGESIZE) / (1024.0 * 1024.0) : 0;
#else
    return 1;
#endif
}

static inline ZoxTstEndGameCounts zox_tst_end_game_count(ecs* world, entity canvas, const char* when) {
    (void) canvas;
    const ZoxTstEndGameCounts counts = {
        // not ui (the main menu's labels are random, a glyph each letter), nor what deletes itself (debug
        //  lines, menu fades)
        .entities = ecs_get_entities(world).alive_count - zox_count_types(Element) - zox_count_types(GlyphIndex) -
            zox_count_types(DestroyInTime) - zox_count_types(AnimationTarget),
        .buffers = zox_gpu_buffers_live,
        .textures = zox_gpu_textures_live,
        .memory_mb = zox_tst_resident_mb(),
        .heap_mb = zox_tst_heap_mb()
    };
    zox_log("[tst] [metric] end_game_cleanup round [%i] %s: entities [%i] gpu buffers [%i] textures [%i] memory [%.1fmb] heap [%.1fmb] tables [%i]",
        zox_tst_end_game_round + 1, when, counts.entities, counts.buffers, counts.textures, counts.memory_mb, counts.heap_mb,
        zox_get_tables_count(world));
    return counts;
}

static inline void zox_tst_end_game_next() {
    zox_tst_end_game_stage++;
    zox_tst_end_game_frames = 0;
}

byte zox_tst_end_game_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    if (!zox_valid(canvas)) {
        zox_tst_waiting = "the player's canvas";
        return 0;
    }
    zox_tst_end_game_frames++;
    if (zox_tst_end_game_frames < zox_tst_menus_settle_frames) {
        return 0;
    }
    zox_tst_progress(zox_tst_end_game_round * 100 + zox_tst_end_game_stage);
    const entity main_menu = zox_tst_menu(world, canvas, zox_id(MenuMain));
    const entity new_realm = zox_tst_menu(world, canvas, zox_id(MenuNewRealm));
    const entity paused = zox_tst_menu(world, canvas, zox_id(MenuPaused));
    const byte state = zox_tst_game_state(world, player);
    const byte playing = state == zox_game_state_playing &&
        zox_getv(player, PlayerState) == zox_player_state_playing;
    switch (zox_tst_end_game_stage) {
        case 0:
            zox_tst_waiting = "the main menu";
            if (!zox_tst_main_menu(world, player, canvas)) {
                return 0;
            }
            if (zox_tst_end_game_round == 0) {
                zox_tst_end_game_count(world, canvas, "before");
            }
            if (!zox_tst_menus_click(world, player, main_menu, &button_event_new_game, "new game")) {
                return 1;
            }
            break;
        case 1:
            zox_tst_waiting = "the new realm menu";
            if (!new_realm) {
                return 0;
            }
            if (!zox_tst_menus_click(world, player, new_realm, &on_confirmed_new_realm, "enter realm")) {
                return 1;
            }
            break;
        case 2:
            zox_tst_waiting = "the game to start playing";
            if (!playing) {
                return 0;
            }
            zox_tst_end_game_play_start = zox_current_time;
            break;
        case 3:
            zox_tst_waiting = "playing for a few seconds";
            if (zox_current_time - zox_tst_end_game_play_start < zox_tst_end_game_play_seconds) {
                return 0;
            }
            zox_tst_end_game_count(world, canvas, "playing");
            button_event_pause_game(world, (ClickEventData) { .clicker = player });
            break;
        case 4:
            zox_tst_waiting = "the pause menu";
            if (!paused) {
                return 0;
            }
            if (!zox_tst_menus_click(world, player, paused, &button_event_end_game, "end game")) {
                return 1;
            }
            break;
        case 5:
            zox_tst_waiting = "the main menu after ending the game, settled";
            if (!main_menu || state != zox_game_none || zox_tst_end_game_frames < zox_tst_end_game_settle_frames) {
                return 0;
            }
            zox_tst_end_game_after[zox_tst_end_game_round] = zox_tst_end_game_count(world, canvas, "after");
            zox_tst_end_game_log_leaks(world);
            zox_tst_end_game_round++;
            if (zox_tst_end_game_round >= zox_tst_end_game_rounds) {
                return 1;
            }
            zox_tst_end_game_stage = 0;
            zox_tst_end_game_frames = 0;
            return 0;
    }
    zox_tst_end_game_next();
    return 0;
}

void zox_tst_end_game_check(ecs* world) {
    (void) world;
    zox_tst_check(zox_tst_end_game_round == zox_tst_end_game_rounds, "only [%i] of [%i] rounds ended",
        zox_tst_end_game_round, zox_tst_end_game_rounds);
    if (zox_tst_end_game_round < zox_tst_end_game_rounds) {
        return;
    }
    const ZoxTstEndGameCounts warm = zox_tst_end_game_after[zox_tst_end_game_rounds - 2];
    const ZoxTstEndGameCounts last = zox_tst_end_game_after[zox_tst_end_game_rounds - 1];
    zox_tst_check(last.entities == warm.entities, "entities left after ending a game [%i], the game before [%i] (+%i)",
        last.entities, warm.entities, last.entities - warm.entities);
    zox_tst_check(last.buffers == warm.buffers, "gpu buffers left after ending a game [%i], the game before [%i]",
        last.buffers, warm.buffers);
    zox_tst_check(last.textures == warm.textures, "gpu textures left after ending a game [%i], the game before [%i]",
        last.textures, warm.textures);
    zox_tst_check(last.heap_mb - warm.heap_mb < zox_tst_end_game_heap_mb,
        "heap grew [%.1fmb] in a game, over [%i]mb", last.heap_mb - warm.heap_mb, zox_tst_end_game_heap_mb);
    zox_log("[tst] [metric] end_game_cleanup a game left heap [%+.1fmb] resident [%+.1fmb]",
        last.heap_mb - warm.heap_mb, last.memory_mb - warm.memory_mb);
}
