#include "music.c"
#include "playlist.c"

void process_arguments_musics(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "--nomusic") == 0) {
            nomusic = 1;
            zox_log("+ setting enabled [nomusic]");
        }
    }
}

/*void on_boot_musics(ecs* world, entity app) {
    if (nosounds) {
        zox_logv("Sounds are disabled: no music.");
        return;
    }
    zox_geter_value(app, GameLink, entity, game);

    zox_geter_value(game, RealmLink, entity, realm);
    if (!zox_valid(realm)) {
        zox_logw("No realm for musics.");
        return;
    }

    spawn_realm_playlist(world, realm);
}*/
