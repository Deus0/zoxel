void process_arguments_huds(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "--autostart") == 0) {
            zox_autostart = 1;
            zox_log("+ [autostart] enabled");
        }
    }
}
