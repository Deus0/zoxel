// Spawns a new realm, with a new seed, onto the players game
void spawn_new_realm(ecs *world, entity player) {
    entity game = zox_get_parent(world, player);
    lint seed = get_unique_time_seed();
    // the same land every run, i.e. to compare perf tests (zox_tst_seed=666 bash bsh/tst.sh perf_fly)
    const char* tst_seed = getenv("zox_tst_seed");
    if (tst_seed) {
        seed = strtoll(tst_seed, NULL, 10);
    }
    spawn_realm(
        world,
        game,
        seed);
}

void delay_enter_new_realm(ecs *world, entity player);

void button_event_new_game(ecs *world, ClickEventData event) {
    entity player = event.clicker;
    entity canvas = zox_get_link(world, player, CanvasLink);
    if (!zox_valid(canvas)) {
        return;
    }
    entity main_menu = zox_get_child_by_id(world, canvas, zox_id(MenuMain));
    if (zox_valid(main_menu)) {
        close_ui_related_tooltip(world, main_menu);
        zox_delete(main_menu);
    }
    spawn_new_realm(world, player);
    // 3D huds (XR) have no text input for the new realm menu's name and seed: in with the defaults
    delay_event(
        world,
        zox_huds3D ? &delay_enter_new_realm : &delay_spawn_menu_new_realm,
        player,
        0.01);
}

void delay_enter_new_realm(ecs *world, entity player) {
    enter_new_realm(world, player, NULL);
}

// --autostart: skips the menus, and enters a new realm
//  - waits for the player to be parented to its game, as that's deferred on spawn
void autostart_new_realm(ecs *world, entity player) {
    if (!zox_valid(player)) {
        return;
    }
    if (!zox_valid(zox_get_parent(world, player))) {
        delay_event(
            world,
            &autostart_new_realm,
            player,
            0.1);
        return;
    }
    zox_log("+ [autostart] entering a new realm");
    spawn_new_realm(world, player);
    delay_event(
        world,
        &delay_enter_new_realm,
        player,
        0.01);
}
