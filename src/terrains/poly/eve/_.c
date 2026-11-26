#include "mesh.c"

static void poly_game_terrain_event(iter* it) {
    if (zox_game_type != zox_game_mode_poly) {
        return;
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GameState, state);
        if (state->value != zox_game_terrain_spawn) {
            continue;
        }
        const entity realm = zox_get_link(world, e, RealmLink);
        if (zox_valid(realm)) {
            spawn_poly_terrain(world, realm, zox_getv(realm, Seed));
        }
    }
}

static void zox_events_poly_terrains(ecs* world) {
    zox_on_add(poly_mesh_event, [in] poly_terrains.PolyHeights, [in] poly_terrains.PolyCells, [in] core.Size2,
        [none] poly_terrains.PolyTerrain, [none] core.Dirty);
    zox_on_add(poly_game_terrain_event, [in] games.GameState, [none] games.Game, [none] core.Dirty);
}
