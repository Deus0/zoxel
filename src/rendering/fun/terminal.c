void process_arguments_rendering(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "-e") == 0 || strcmp(args[i], "--opengles") == 0) {
            override_opengl_es = 1;
        } else if (strcmp(args[i], "--nopost") == 0) {
            // cameras draw straight to the screen, no render texture (i.e. comparing gl and vulkan)
            zox_disable_post_processing = 1;
        } else if (strcmp(args[i], "--fog") == 0 && i + 1 < count) {
            // the Fog setting (0 to 1, the menu's), over the settings file's
            fog_density = atof(args[++i]) * fog_settings_mul;
            zox_settings_set_by_args("Fog");
        } else if (strcmp(args[i], "--outlines") == 0 || strcmp(args[i], "--nooutlines") == 0) {
            zox_camera_outlines = args[i][2] == 'o';
            zox_settings_set_by_args("Camera Outlines");
        }
    }
}