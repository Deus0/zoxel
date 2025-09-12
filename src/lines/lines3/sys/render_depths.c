static const color_rgb color_render_depths_invisible = { 255, 0, 0 };

static const color_rgb color_render_depths[] = {
    { 255, 0, 0 },     // 0 - vivid red
    { 0, 255, 0 },     // 1 - bright green
    { 0, 0, 255 },     // 2 - electric blue
    { 255, 255, 0 },   // 3 - blazing yellow
    { 255, 0, 255 },   // 4 - magenta shock
    { 0, 255, 255 },   // 5 - cyan glare
    { 255, 165, 0 },   // 6 - orange fire
    { 128, 0, 128 },   // 7 - deep purple
};

void RenderDepthColorSystem(ecs_iter_t *it) {
    zox_sys_world()
    zox_sys_begin()
    zox_sys_in(DebugCubeLines)
    zox_sys_in(RenderDepth)
    zox_sys_in(RenderDisabled)
    zox_sys_out(Color)
    for (int i = 0; i < it->count; i++) {
        zox_sys_e()
        zox_sys_i(DebugCubeLines, debugCubeLines)
        zox_sys_i(RenderDepth, renderDepth)
        zox_sys_i(RenderDisabled, renderDisabled)
        zox_sys_o(Color, colorr)
        const byte mode = debugCubeLines->value;
        if (!mode) {
            continue;
        }
        color_rgb output = color_rgb_black;
        if (mode == zox_cubeline_debug_render_disabled) {
            if (renderDisabled->value) {
                output = (color_rgb) { 255, 0, 0 };
            } else {
                output = (color_rgb) { 0, 155, 0 };
            }
        } else if (mode == zox_cubeline_debug_verts) {
            if (zox_has(e, MeshIndicies)) {
                zox_geter(e, MeshIndicies, meshIndicies)
                if (meshIndicies->length == 0) {
                    output = (color_rgb) { 255, 0, 0 };
                } else {
                    output = (color_rgb) { 0, 255, 0 };
                }
            } else {
                output = (color_rgb) { 155, 155, 0 };
            }
        } else if (mode == zox_cubeline_debug_render_depths) {
            if (renderDepth->value == render_depth_spawning || renderDepth->value == render_depth_invisible) {
                output = color_render_depths_invisible;
            } else if (renderDepth->value < 6) {
                output = color_render_depths[renderDepth->value];
            }

            /*else if (renderDepth->value == 0) {
                output = (color_rgb) { 0, 255, 0 };
            } else if (renderDepth->value == 1) {
                output = (color_rgb) { 0, 0, 255 };
            } else if (renderDepth->value == 2) {
                output = (color_rgb) { 255, 255, 0 };
            } else if (renderDepth->value == 3) {
                output = (color_rgb) { 255, 0, 255 };
            } else if (renderDepth->value == 4) {
                output = (color_rgb) { 0, 255, 255 };
            } else if (renderDepth->value == 5) {
                output = (color_rgb) { 122, 122, 122 };
            } else {
                output = (color_rgb) { 255, 255, 255 };
            }*/
        } else {
            continue;
        }
        colorr->value = color_rgb_to_color(output);
    }
} zoxd_system(RenderDepthColorSystem)
