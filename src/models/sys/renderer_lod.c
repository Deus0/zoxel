// Sets an owners renderer child to the lod mesh at its RenderDepth
void renderer_lod_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepthDirty);
    zox_sys_in(RenderDepth);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepthDirty, dirty);
        zox_sys_i(RenderDepth, rdepth);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        const entity renderer = zox_get_link(world, e, RendererLink);
        if (!zox_valid(renderer)) {
            continue;
        }
        entity mesh = get_owner_lod_mesh(world, e, rdepth->value);
        zox_set_link(world, renderer, InstanceLink, mesh);
    }
    zox_sys_on_end();
} zoxd_system(renderer_lod_system);
