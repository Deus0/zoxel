zox_sys2(CharacterRenderDepthSystem) {
    zox_sys_begin();
    zox_sys_in(RenderDepthDirty);
    zox_sys_out(BuildChunkMesh);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(RenderDepthDirty, dirty);
        zox_sys_o(BuildChunkMesh, build_mesh);
        if (dirty->value == zox_dirty_active) {
            build_mesh->value = zox_dirty_trigger;
            // TODO: set mesh instance link by renderDepth?
        }
    }
} zox_sys_end(CharacterRenderDepthSystem);
