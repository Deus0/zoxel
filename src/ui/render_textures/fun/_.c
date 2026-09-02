void on_set_viewport_scale(ecs* world, entity app) {
    // TODO: Apply to actual viewport??
#ifdef zox_safety_checks
    if (!zox_valid(app)) {
        zox_loge("App invalid in RenderSettings");
        return;
    }
#endif
    zox_setv(app, WindowSizeDirty, zox_dirty_trigger);
    entity canvas = zox_get_link(world, app, Canvas);
    if (!zox_valid(canvas)) {
        zox_logw("App has no Canvas in RenderSettings");
        return;
    }
    entity render_texture = zox_get_child_by_id(world, canvas, zox_id(RenderTexture));
    if (!zox_valid(render_texture)) {
        zox_loge("App Canvas RenderTexture Invalid in RenderSettings");
        return;
    }
    zox_setv(render_texture, LayoutSizeDirty, zox_dirty_trigger);
}
