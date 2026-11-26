void process_arguments_post_processing(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "--outlines") == 0 || strcmp(args[i], "--nooutlines") == 0) {
            zox_camera_outlines = args[i][2] == 'o';
            zox_settings_set_by_args("Camera Outlines");
        } else if (strcmp(args[i], "--tonemap") == 0 && i + 1 < count) {
            // the Tonemap setting (0 clamp, 1 soft, 2 filmic), over the settings file's
            zox_tonemap = (byte) atoi(args[++i]);
            zox_settings_set_by_args("Tonemap");
        } else if (strcmp(args[i], "--bloom") == 0 || strcmp(args[i], "--nobloom") == 0) {
            zox_bloom = args[i][2] == 'b';
            zox_settings_set_by_args("Bloom");
        }
    }
}
