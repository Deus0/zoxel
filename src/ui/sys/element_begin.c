// NOTE: Initializes Mesh based on instantiated layout size
zox_sys2(ElementBeginSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Initialize);
    zox_sys_in(LayoutSize);
    zox_sys_in(MeshAlignment);
    zox_sys_out(MeshVertices2D);
    zox_sys_out(MeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Initialize, initialize);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(MeshAlignment, meshAlignment);
        zox_sys_o(MeshDirty, dirty);
        zox_sys_o(MeshVertices2D, verts);
        if (initialize->value != zox_dirty_active) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            zox_loge("Invalid Canvas for Layout %s", zox_get_name(e));
            continue;
        }
        // we also set mesh verts here
        zox_geter_value(canvas, LayoutSize, int2, canvas_size);
        const float2 canvas_sizef = int2_to_float2(canvas_size);
        const float2 size2D = (float2) { size->value.x / canvas_sizef.y, size->value.y / canvas_sizef.y };
        set_mesh_vertices_scale2D(verts, get_aligned_mesh2D(meshAlignment->value), 4, size2D);
        dirty->value = mesh_state_trigger;
        // spawn gpu bufers
        // spawn mesh buffers needs to be done on main thread
        // meshGPULink->value = spawn_gpu_mesh_buffers();
        // uvsGPULink->value = zox_gpu_create_buffer();
        if (dbg_log) {
            zox_log("Element Mesh Initialized [%s]", zox_getn(e));
        }
    }
} zox_sys_end(ElementBeginSystem);
