/*void print_terrain_settings() {
    zox_log(" > terrain settings set")
    // zox_log("     + terrain seed is [%i]\n", (int) terrain_seed)
    zox_log("     + terrain_depth is [%i]", terrain_depth)
    // zox_log("     + render distance is [%i]", terrain_spawn_distance)
    zox_log("     + terrain vertical is [%i]", terrain_vertical)
    zox_log("     + height noise frequency is [%f]", terrain_frequency)
    zox_log("     + lowest low is [%i]", lowest_voxel_height)
}*/

// args win over the settings file (see zox_settings_set_by_args)
static inline void zox_terrain_size_set_by_args() {
    if (terrain_lod_near > terrain_lod_far) {
        terrain_lod_near = terrain_lod_far;
    }
    zox_settings_set_by_args("Far RD");
    zox_settings_set_by_args("Near RD");
    zox_settings_set_by_args("Vertical RD");
}

void process_arguments_terrain(ecs *world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "--tiny") == 0) {
            terrain_lod_far = 3;
            render_distance_y = 3;
            zox_log("+ [tiny terrain] [%ix%i]", terrain_lod_far, render_distance_y)
            zox_terrain_size_set_by_args();
        } else if (strcmp(args[i], "--medium") == 0) {
            terrain_lod_far = 6;
            render_distance_y = 3;
            zox_log("+ [medium terrain] [%ix%i]", terrain_lod_far, render_distance_y)
            zox_terrain_size_set_by_args();
        } else if (strcmp(args[i], "--far") == 0 && i + 1 < count) {
            // a far distance of its own, i.e. tests on the user's (--stress --far 8)
            terrain_lod_far = (byte) atoi(args[++i]);
            zox_log("+ [far terrain] [%i]", terrain_lod_far)
            zox_terrain_size_set_by_args();
        } else if (strcmp(args[i], "--vertical") == 0 && i + 1 < count) {
            // chunks streamed up and down, the land's highest is this many chunks tall (heights/fun, heights are bytes:
            //  past 255 blocks only more sky)
            render_distance_y = (byte) atoi(args[++i]);
            zox_log("+ [vertical terrain] [%i]", render_distance_y)
            zox_terrain_size_set_by_args();
        } else if (strcmp(args[i], "--large") == 0) {
            terrain_lod_far = 12;
            render_distance_y = 4;
            zox_log("+ [large terrain] [%ix%i]", terrain_lod_far, render_distance_y)
            zox_terrain_size_set_by_args();
        }
    }
}
