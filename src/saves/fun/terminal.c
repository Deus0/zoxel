// --nosaves: saves go to a throwaway folder, so runs (tests) don't fill the real save folder
//  - a test's setup can set zox_saves_root first, --nosaves keeps it
//  - bash bsh/tst.sh passes it, and deletes the folder after each run
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
    }
}
