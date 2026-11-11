// tst: music_crossfade, the main menu plays the imported song, it crossfades with the game's music
//  - from boot the game plays its menu music, the imported song (not the generated fallback)
//  - starting a game crossfades to the realm's music: the menu music fades out and stops
//  - ending the game crossfades back to the menu music
//  - run: bash bsh/tst.sh music_crossfade
int zox_tst_music_stage = 0;
int zox_tst_music_frames = 0;
double zox_tst_music_time = 0;

static inline entity zox_tst_music_playing(ecs* world, entity game) {
    return zox_get_link(world, game, MusicPlayingLink);
}

// the music is on, and has faded all the way in
static inline byte zox_tst_music_full(ecs* world, entity music) {
    return zox_valid(music) && zox_getv(music, MusicEnabled) && zox_getv(music, MusicVolume) >= 1;
}

// the music has faded out and stopped
static inline byte zox_tst_music_stopped(ecs* world, entity music) {
    return zox_valid(music) && !zox_getv(music, MusicEnabled) && zox_getv(music, MusicVolume) <= 0;
}

byte zox_tst_music_crossfade_ready(ecs* world) {
    const entity player = dbg_player;
    const entity canvas = zox_valid(player) ? zox_get_link(world, player, CanvasLink) : 0;
    const entity game = zox_valid(player) ? zox_get_link(world, player, GameLink) : 0;
    if (!zox_valid(canvas) || !zox_valid(game)) {
        zox_tst_waiting = "the player's canvas and game";
        return 0;
    }
    zox_tst_music_frames++;
    const entity menu_music = zox_get_link(world, game, MenuMusicLink);
    const entity playing = zox_tst_music_playing(world, game);
    switch (zox_tst_music_stage) {
        case 0: {
            static char waiting[160];
            snprintf(waiting, sizeof(waiting), "the menu music to fade in (menu music [%s] playing [%s] enabled [%i] volume [%.2f])",
                zox_valid(menu_music) ? zox_getn(menu_music) : "none", zox_valid(playing) ? zox_getn(playing) : "none",
                zox_valid(menu_music) ? zox_getv(menu_music, MusicEnabled) : -1,
                zox_valid(menu_music) ? zox_getv(menu_music, MusicVolume) : -1.0f);
            zox_tst_waiting = waiting;
            const entity main_menu = zox_tst_main_menu(world, player, canvas);
            if (!main_menu || !zox_tst_music_full(world, menu_music)) {
                return 0;
            }
            zox_tst_check(playing == menu_music, "the main menu plays [%s], not its menu music", zox_valid(playing) ? zox_getn(playing) : "nothing");
            zox_tst_check(!zox_has(menu_music, GenerateMusic) || !zox_getv(menu_music, GenerateMusic),
                "the menu music is the generated fallback, the imported song didn't load");
            zox_tst_check(zox_get_children_count(world, menu_music) > 0, "the menu music has no notes");
            if (!zox_tst_menus_click(world, player, main_menu, &button_event_new_game, "new game")) {
                return 1;
            }
            break;
        }
        case 1: {
            zox_tst_waiting = "the new realm menu";
            const entity new_realm = zox_get_child_by_id(world, canvas, zox_id(MenuNewRealm));
            if (!zox_valid(new_realm)) {
                return 0;
            }
            if (!zox_tst_menus_click(world, player, new_realm, &on_confirmed_new_realm, "enter realm")) {
                return 1;
            }
            break;
        }
        case 2:
            zox_tst_waiting = "playing, crossfaded to the game's music";
            if (zox_getv(player, PlayerState) != zox_player_state_playing ||
                playing == menu_music ||
                !zox_tst_music_full(world, playing) ||
                !zox_tst_music_stopped(world, menu_music))
            {
                return 0;
            }
            zox_log("[tst] in game, playing [%s], the menu music stopped", zox_getn(playing));
            button_event_pause_game(world, (ClickEventData) { .clicker = player });
            break;
        case 3: {
            zox_tst_waiting = "the pause menu";
            const entity paused = zox_get_child_by_id(world, canvas, zox_id(MenuPaused));
            if (!zox_valid(paused)) {
                return 0;
            }
            if (!zox_tst_menus_click(world, player, paused, &button_event_end_game, "end game")) {
                return 1;
            }
            break;
        }
        default:
            zox_tst_waiting = "the menu music to fade back in";
            if (playing != menu_music || !zox_tst_music_full(world, menu_music)) {
                return 0;
            }
            return 1;
    }
    zox_tst_music_stage++;
    zox_tst_music_frames = 0;
    return 0;
}

void zox_tst_music_crossfade_check(ecs* world) {
    (void) world;
}
