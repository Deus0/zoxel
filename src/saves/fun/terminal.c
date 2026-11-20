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
            // one a run (its pid), in the temp folder: windows' %TEMP% is git bash's /tmp, so tst.sh deletes both
            //  (it was one zoxel_nosaves folder beside the game on windows, every test saw the ones before's saves)
#ifdef zox_windows
            const char* temp = getenv("TEMP");
            snprintf(zox_saves_root, sizeof(zox_saves_root), "%s/zoxel_nosaves_%i", temp && temp[0] ? temp : ".", (int) getpid());
#else
            snprintf(zox_saves_root, sizeof(zox_saves_root), "/tmp/zoxel_nosaves_%i", (int) getpid());
#endif
            zox_mkdir(zox_saves_root);
        }
        zox_log("+ [nosaves] saving to [%s]", zox_saves_root);
        const char* real_settings = getenv("zox_tst_settings");
        if (real_settings && real_settings[0]) {
            char settings[1024];
            snprintf(settings, sizeof(settings), "%s/settings.cfg", zox_saves_root);
            if (!zox_copy_file(real_settings, settings)) {
                zox_loge("[nosaves] couldn't copy settings [%s]", real_settings);
            } else {
                zox_log("+ [nosaves] settings are a copy of [%s]", real_settings);
            }
        }
    }
}
