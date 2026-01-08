void arguments_ui(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "-tu") == 0 || strcmp(args[i], "--tinyui") == 0) {
            zox_log("Tiny UI Mode Enabled");
            ui_scale = 1;
        } else if (strcmp(args[i], "-su") == 0 || strcmp(args[i], "--smallui") == 0) {
            zox_log("Small UI Mode Enabled");
            ui_scale = 2;
        }
    }
}