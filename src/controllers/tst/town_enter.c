// tst: town_enter, walking into a town greets you and plays its music, leaving plays the overworld's
//  - puts the player's character out past a town's walls, then over its middle, then out again (held, no falling;
//    it may start in that town, its toast long gone)
//  - in: its character has InTown, TownLink that town, a toast "Welcome to <its name>" (tst/town_enter.ppm), the realm's
//      town track (1) playing
//  - out: no InTown, no TownLink, the overworld track (0) playing
//  - run: bash bsh/tst.sh town_enter
int zox_tst_te_stage = 0;
double zox_tst_te_time = 0;
entity zox_tst_te_town = 0;
byte zox_tst_te_in = 0, zox_tst_te_linked = 0, zox_tst_te_out = 0, zox_tst_te_unlinked = 0;
int zox_tst_te_track_in = -1, zox_tst_te_track_out = -1;
char zox_tst_te_toast[96];

// the realm's track playing: its index in the realm's playlist, -1 for none
static inline int zox_tst_te_track(ecs* world, entity realm) {
    const entity playing = zox_get_link(world, zox_get_link(world, realm, GameLink), MusicPlayingLink);
    const entity playlist = zox_get_child_by_id(world, realm, zox_id(Playlist));
    const MusicLinks* musics = zox_valid(playlist) ? zox_get(playlist, MusicLinks) : NULL;
    for (int i = 0; musics && i < musics->length; i++) {
        if (musics->value[i] == playing) {
            return i;
        }
    }
    return -1;
}

// the toast's text, empty if there's none
static inline void zox_tst_te_read_toast(ecs* world, entity canvas) {
    zox_tst_te_toast[0] = 0;
    const entity toast = zox_get_child_by_id(world, canvas, zox_id(Toast));
    if (!zox_valid(toast)) {
        return;
    }
    iter it = zox_children(world, toast);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            if (zox_has(it.entities[i], TextData)) {
                char* text = get_zext_text(zox_get(it.entities[i], TextData));
                snprintf(zox_tst_te_toast, sizeof(zox_tst_te_toast), "%s", text ? text : "");
                free(text);
            }
        }
    }
}

byte zox_tst_town_enter_ready(ecs* world) {
    if (!zox_tst_te_stage) {
        if (!zox_tst_player_spawn_ready(world)) {
            return 0;
        }
        const entity region = zox_tst_player_region(world);
        entity towns[zox_children_capacity];
        const uint count = zox_valid(region) ? zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town)) : 0;
        if (!count) {
            zox_tst_check(0, "no town in the player's region");
            return 1;
        }
        zox_tst_te_town = towns[0];
        zox_tst_te_stage = 1;
        zox_tst_te_time = zox_current_time;
    }
    zox_tst_waiting = "in and out of a town";
    zox_tst_progress(zox_tst_te_stage);
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity realm = zox_get_link(world, character, RealmLink);
    const float scale = zox_getv(zox_get_link(world, character, TerrainLink), BlockScale);
    const int2 center = zox_getv(zox_tst_te_town, BlockPosition2);
    const byte2 size = zox_getv(zox_tst_te_town, TownSize);
    // out past its walls (a side not in another town), in its middle (stage 2), out again, high up (held, no falling)
    int2 at = center;
    if (zox_tst_te_stage != 2) {
        const int2 sides[] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } };
        for (int s = 0; s < 4; s++) {
            at = (int2) { center.x + sides[s].x * (size.x / 2 + 24), center.y + sides[s].y * (size.y / 2 + 24) };
            if (!town_at(world, at)) {
                break;
            }
        }
    }
    zox_setv(character, Position3D, ((float3) { (at.x + 0.5f) * scale, 90 * scale, (at.y + 0.5f) * scale }));
    zox_setv(character, Velocity3D, float3_zero);
    const double since = zox_current_time - zox_tst_te_time;
    if (zox_tst_te_stage == 1) {
        // until it's out (it may start in that town, spawning holds it there a while)
        if (since < 0.6 || (zox_has(character, InTown) && since < 10)) {
            return 0;
        }
        zox_tst_te_stage = 2;
        zox_tst_te_time = zox_current_time;
        return 0;
    }
    if (zox_tst_te_stage == 2) {
        // until its toast has popped in
        const entity toast = zox_get_child_by_id(world, zox_get_link(world, dbg_player, CanvasLink), zox_id(Toast));
        if (since < 0.4 || (!zox_valid(toast) && since < 3)) {
            return 0;
        }
        zox_tst_te_in = zox_has(character, InTown);
        zox_tst_te_linked = zox_get_link(world, character, TownLink) == zox_tst_te_town;
        zox_tst_te_read_toast(world, zox_get_link(world, dbg_player, CanvasLink));
        zox_tst_te_track_in = zox_tst_te_track(world, realm);
        snprintf(zox_screenshot_request, sizeof(zox_screenshot_request), "%s", zox_tst_path("town_enter.ppm"));
        zox_tst_te_stage = 3;
        zox_tst_te_time = zox_current_time;
        return 0;
    }
    if (since < 0.6) {
        return 0;
    }
    zox_tst_te_out = !zox_has(character, InTown);
    zox_tst_te_unlinked = !zox_get_link(world, character, TownLink);
    zox_tst_te_track_out = zox_tst_te_track(world, realm);
    return 1;
}

void zox_tst_town_enter_check(ecs* world) {
    const char* name = zox_valid(zox_tst_te_town) ? zox_getv(zox_tst_te_town, ZoxName) : "";
    char expected[96];
    snprintf(expected, sizeof(expected), "Welcome to %s", name[0] ? name : "Town");
    zox_log("[tst] in town: the toast [%s], track [%i]; out: track [%i]", zox_tst_te_toast, zox_tst_te_track_in, zox_tst_te_track_out);
    zox_tst_check(zox_tst_te_in, "the character in the town isn't InTown");
    zox_tst_check(zox_tst_te_linked, "the character in the town isn't linked to it (TownLink)");
    zox_tst_check(!strcmp(zox_tst_te_toast, expected), "the toast says [%s], not [%s]", zox_tst_te_toast, expected);
    zox_tst_check(zox_tst_te_track_in == 1, "in the town track [%i] plays, not the town's (1)", zox_tst_te_track_in);
    zox_tst_check(zox_tst_te_out, "the character out of the town is still InTown");
    zox_tst_check(zox_tst_te_unlinked, "the character out of the town still has its TownLink");
    zox_tst_check(zox_tst_te_track_out == 0, "out of the town track [%i] plays, not the overworld's (0)", zox_tst_te_track_out);
}
