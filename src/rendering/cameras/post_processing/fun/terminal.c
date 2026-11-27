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
        } else if (strcmp(args[i], "--hdr") == 0 || strcmp(args[i], "--nohdr") == 0) {
            // the HDR setting: hdr10 or scrgb if the screen offers either (fun/output.c), asked before the window's made
            zox_hdr_output_asked = args[i][2] == 'h' ? zox_hdr_output_hdr10 : zox_hdr_output_off;
            zox_settings_set_by_args("HDR");
        } else if (strcmp(args[i], "--hdr-canvas") == 0) {
            // hdr output's canvas and encode draw over an 8 bit swapchain, passed through: looks as off (a test of it)
            zox_hdr_output_asked = zox_hdr_output_canvas;
            zox_settings_set_by_args("HDR");
        }
    }
}
