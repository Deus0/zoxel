// Sets an owners renderer child to the lod mesh at its RenderDepth
zox_sys2(RendererLodSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepthDirty);
    zox_sys_in(RenderDepth);
    zox_sys_in(RendererLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepthDirty, dirty);
        zox_sys_i(RenderDepth, rdepth);
        zox_sys_i(RendererLink, renderer);
        if (dirty->value != zox_dirty_active || !zox_valid(renderer->value)) {
            continue;
        }
        entity mesh = get_owner_lod_mesh(world, e, rdepth->value);
        zox_setv(renderer->value, InstanceLink, mesh);
    }
} zox_sys_end(RendererLodSystem);
