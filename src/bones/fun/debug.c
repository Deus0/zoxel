// The bone debug view: the bones' gizmos (lines to their parents, boxes round their parts in their paint) over the
//  skeleton meshes see through (Transparent Mesh) and painted by bone (Paint Weights), to see where each bone sits in
//  its body and which skin it moves as it animates
//  - toggled in the Gizmos debug ui (g, "Bones"), or --bones (tests: bash bsh/tst.sh body_views --bones)
static inline void bones_debug_set(byte enabled) {
    render_bones = enabled;
    transparent_meshes = enabled;
    paint_weights = enabled;
}

void bones_debug_toggle(ecs* world, ClickEventData event) {
    (void) world;
    (void) event;
    bones_debug_set(!render_bones);
}

void process_arguments_bones(ecs* world, char* args[], int count) {
    (void) world;
    for (int i = 1; i < count; i++) {
        if (strcmp(args[i], "--bones") == 0) {
            bones_debug_set(1);
            zox_settings_set_by_args("Bone Gizmos");
            zox_settings_set_by_args("Transparent Mesh");
            zox_settings_set_by_args("Paint Weights");
        }
    }
}
