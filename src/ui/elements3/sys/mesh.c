void set_mesh_vertices_scale3(MeshVertices* verts, const float2* new_verts, int length, float2 scale) {
    resize_MeshVertices(verts, length);
    for (int i = 0; i < length; i++) {
        verts->value[i] = (float3) {
            new_verts[i].x * scale.x,
            new_verts[i].y * scale.y,
            0
        };
    }
}

// Initialize Mesh2D data when size changes
zox_sys2(Layout3MeshUpdateSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSizeDirty);
    zox_sys_in(LayoutSize);
    zox_sys_in(MeshAlignment);
    zox_sys_out(MeshVertices);
    zox_sys_out(MeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSizeDirty, dirty);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(MeshAlignment, alignment);
        zox_sys_o(MeshVertices, verts);
        zox_sys_o(MeshDirty, mesh_dirty);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas3));
        if (!zox_valid(canvas)) {
            zox_loge("Invalid Canvas for Layout %s", zox_get_name(e));
            continue;
        }
        float2 canvas_size = int2_to_float2(zox_getv(canvas, LayoutSize));
        float2 size2 = (float2) {
            (size->value.x / canvas_size.y) * 0.5f,
            (size->value.y / canvas_size.y) * 0.5f
        };
        set_mesh_vertices_scale3(verts, get_aligned_mesh2D(alignment->value), 4, size2);
        mesh_dirty->value = mesh_state_upload;
        if (dbg_log) {
            zox_log("Layout3MeshUpdateSystem [%s] Layout Size Dirty, Updated Mesh", zox_getn(e));
        }
    }
} zox_sys_end(Layout3MeshUpdateSystem);

// NOTE: Initializes Mesh based on instantiated layout size
zox_sys2(Layout3MeshBeginSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Initialize);
    zox_sys_in(LayoutSize);
    zox_sys_in(MeshAlignment);
    zox_sys_out(MeshVertices);
    zox_sys_out(MeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Initialize, initialize);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(MeshAlignment, alignment);
        zox_sys_o(MeshDirty, dirty);
        zox_sys_o(MeshVertices, verts);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas3));
        if (!zox_valid(canvas)) {
            zox_loge("Invalid Canvas3 for Layout %s", zox_get_name(e));
            continue;
        }
        // we also set mesh verts here
        float2 canvas_size = int2_to_float2(zox_getv(canvas, LayoutSize));
        float2 size2 = (float2) {
            (size->value.x / canvas_size.y) * 0.5f,
            (size->value.y / canvas_size.y) * 0.5f
        };
        set_mesh_vertices_scale3(verts, get_aligned_mesh2D(alignment->value), 4, size2);
        dirty->value = mesh_state_upload;
        if (dbg_log) {
            zox_log("Layout3MeshBeginSystem Mesh Updated [%s]", zox_getn(e));
        }
    }
} zox_sys_end(Layout3MeshBeginSystem);

