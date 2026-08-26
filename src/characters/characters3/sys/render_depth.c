zox_sys2(CharacterRenderDepthSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDepthDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDepthDirty, dirty);
        if (dirty->value == zox_dirty_active) {
            zox_add(e, BuildMesh);
            // build_mesh->value = zox_build_chunk_mesh_run;
            // TODO: set mesh instance link by renderDepth?
        }
    }
} zox_sys_end(CharacterRenderDepthSystem);
