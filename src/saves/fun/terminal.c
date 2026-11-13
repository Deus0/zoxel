// --nosaves: saves go to a throwaway folder, so runs (tests) don't fill the real save folder
//  - a test's setup can set zox_saves_root first, --nosaves keeps it
//  - bash bsh/tst.sh passes it, and deletes the folder after each run
//  - zox_tst_settings=<settings.cfg>: copied in as its settings, to run like real play (i.e. the user's Block Res)
void process_arguments_saves(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "--nosaves") != 0) {
            continue;
        }
        if (!zox_saves_root[0]) {
#ifdef zox_windows
            snprintf(zox_saves_root, sizeof(zox_saves_root), "zoxel_nosaves");
            mkdir(zox_saves_root);
#else
            snprintf(zox_saves_root, sizeof(zox_saves_root), "/tmp/zoxel_nosaves_%i", (int) getpid());
            mkdir(zox_saves_root, 0755);
#endif
        }
        zox_log("+ [nosaves] saving to [%s]", zox_saves_root);
        const char* real_settings = getenv("zox_tst_settings");
        if (real_settings && real_settings[0]) {
            char command[1024];
            snprintf(command, sizeof(command), "cp '%s' '%s/settings.cfg'", real_settings, zox_saves_root);
            if (system(command) != 0) {
                zox_loge("[nosaves] couldn't copy settings [%s]", real_settings);
            } else {
                zox_log("+ [nosaves] settings are a copy of [%s]", real_settings);
            }
        }
    }
}
