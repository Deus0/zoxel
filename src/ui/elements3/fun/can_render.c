// gets parent in tree's UI Holder, uses its render LOD to check if can render a 3D UI
byte can_render_ui(ecs* world, entity e) {
    if (!zox_valid(e)) {
        return 0;
    }
    if (zox_has(e, UIHolderLink)) {
        zox_geter(e, UIHolderLink, uiHolderLink);
        if (zox_valid(uiHolderLink->value)) {
            if (zox_has(uiHolderLink->value, RenderDepth) && zox_has(uiHolderLink->value, MaxRenderDepth)) {
                zox_geter_value(uiHolderLink->value, RenderDepth, byte, render_depth);
                // TODO: Use a UIVisible flag
                zox_geter_value(uiHolderLink->value, MaxRenderDepth, byte, max_render_depth);
                byte render_depth_valid =
                    render_depth != render_depth_uninitialized &&
                    // render_depth != render_depth_invisible &&
                    render_depth >= max_render_depth - 2;
                if (!render_depth_valid) {
                    return 0;
                }
            }
        }
    }
    entity parent = zox_get_parent(world, e);
    if (zox_valid(parent)) {
        return can_render_ui(world, parent);
    }
    return 1;
}
