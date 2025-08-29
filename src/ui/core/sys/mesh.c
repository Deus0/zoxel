void LayoutMeshSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(LayoutSizeDirty);
    zox_sys_in(CanvasLink);
    zox_sys_in(LayoutSize);
    zox_sys_in(MeshAlignment);
    zox_sys_out(MeshVertices2D);
    zox_sys_out(MeshDirty);

    for (int i = 0; i < it->count; i++) {

        zox_sys_i(LayoutSizeDirty, dirty);
        zox_sys_i(CanvasLink, canvas);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(MeshAlignment, alignment);
        zox_sys_o(MeshVertices2D, verts);
        zox_sys_o(MeshDirty, mesh_dirty);

        if (dirty->value != zox_dirty_active) {
            continue;
        }
        const float2 canvas_size = int2_to_float2(zox_gett_value(canvas->value, LayoutSize));
        const float2 sizef = (float2) {
            size->value.x / canvas_size.y,
            size->value.y / canvas_size.y };

        set_mesh_vertices_scale2D(
            verts,
            get_aligned_mesh2D(alignment->value),
            4,
            sizef);
        mesh_dirty->value = mesh_state_trigger;

        /*zox_sys_e();
        zox_log("+ [%s] Mesh Rescaled: %fx%f - og [%ix%i]",
            zox_get_name(e),
            sizef.x, sizef.y,
            size->value.x, size->value.y);*/
    }
} zoxd_system2(LayoutMeshSystem);