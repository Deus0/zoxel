// tst: boot_menu, zoxel boots into its start menu (eve/, on zox_game_boot), once, and no other menu
//  - the game went boot, then none (the menu)
//  - run: bash bsh/tst.sh boot_menu
double zox_tst_bm_time = 0;

byte zox_tst_boot_menu_ready(ecs* world) {
    zox_tst_waiting = "the start menu";
    const entity player = zox_players[0];
    if (!zox_valid(player) || !zox_valid(zox_get_link(world, player, MenuStart))) {
        return 0;
    }
    if (!zox_tst_bm_time) {
        zox_tst_bm_time = zox_current_time;
    }
    return zox_current_time - zox_tst_bm_time > 1;     // a second on: no second menu came
}

void zox_tst_boot_menu_check(ecs* world) {
    const entity player = zox_players[0];
    const entity canvas = zox_get_link(world, player, CanvasLink);
    int starts = 0;
    iter it2 = zox_children(world, canvas);
    while (zox_children_next(it2)) {
        for (int j = 0; j < it2.count; j++) {
            starts += zox_has(it2.entities[j], MenuStart);
        }
    }
    zox_tst_check(starts == 1, "[%i] start menus, expected 1", starts);
    zox_tst_check(zox_tst_menus_open(world, canvas) == 0, "[%i] other menus open", zox_tst_menus_open(world, canvas));
    zox_tst_check(zox_getv(local_game, GameState) == zox_game_none, "game state [%i], not none",
        zox_getv(local_game, GameState));
    zox_tst_check(zox_getv(local_game, LastGameState) == zox_game_boot, "the last game state [%i], not boot",
        zox_getv(local_game, LastGameState));
}

// tst: zoxel_solar, a realm generating gets its sun in its solar step (sys/solar.c), its sunset from its seed;
//  generated again it has its one sun, not two
//  - run: bash bsh/tst.sh zoxel_solar
entity zox_tst_solar_realm = 0;
byte zox_tst_solar_generated = 0;
uint zox_tst_solar_suns = 0;
color_rgb zox_tst_solar_sunset;

byte zox_tst_zoxel_solar_ready(ecs* world) {
    zox_tst_waiting = "the realm generated twice";
    if (!zox_tst_solar_realm) {
        zox_tst_solar_realm = zox_ins(world, prefab_realm);
        zox_setv(zox_tst_solar_realm, Seed, 1234);
        zox_setv(zox_tst_solar_realm, GenerateRealm, zox_generate_realm_start);
        return 0;
    }
    zox_tst_progress(zox_tst_solar_generated * 100 + zox_getv(zox_tst_solar_realm, GenerateRealm));
    if (zox_getv(zox_tst_solar_realm, GenerateRealm)) {
        return 0;
    }
    if (++zox_tst_solar_generated == 1) {
        zox_setv(zox_tst_solar_realm, GenerateRealm, zox_generate_realm_start);
        return 0;
    }
    entity suns[4];
    zox_tst_solar_suns = zox_get_children_by_id(world, zox_tst_solar_realm, suns, 4, zox_id(Sun));
    if (zox_tst_solar_suns) {
        zox_tst_solar_sunset = zox_getv(suns[0], SecondaryColorRGB);
    }
    return 1;
}

void zox_tst_zoxel_solar_check(ecs* world) {
    (void) world;
    const color_rgb s = zox_tst_solar_sunset, seeded = zoxel_sunset_color(1234);
    zox_tst_check(zox_tst_solar_suns == 1, "the realm generated twice has [%u] suns, not its one", zox_tst_solar_suns);
    zox_tst_check(s.r == seeded.r && s.g == seeded.g && s.b == seeded.b, "its sun's sunset [%i %i %i] isn't its seed's [%i %i %i]",
        s.r, s.g, s.b, seeded.r, seeded.g, seeded.b);
}

void zox_tsts_zoxel(ecs* world) {
    (void) world;
    zox_tst_add("boot_menu", zox_tst_boot_menu_ready, zox_tst_boot_menu_check, 60);
    zox_tst_set_setup("boot_menu", NULL, 1);
    zox_tst_add("zoxel_solar", zox_tst_zoxel_solar_ready, zox_tst_zoxel_solar_check, 60);
}
