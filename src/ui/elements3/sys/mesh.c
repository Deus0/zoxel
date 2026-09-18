void set_mesh_vertices_scale3(
    MeshVertices* verts,
    const float2* new_verts,
    int length,
    float2 scale)
{
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
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSizeDirty, dirty);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(MeshAlignment, alignment);
        zox_sys_o(MeshVertices, verts);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas3));
        if (!zox_valid(canvas)) {
            zox_loge("[Layout3MeshUpdateSystem] Invalid [canvas]: %s", zox_getn(e));
            continue;
        }
        float2 canvas_size = int2_to_float2(zox_getv(canvas, LayoutSize));
        float2 scale = (float2) {
            (size->value.x / canvas_size.y), // * 0.5f,
            (size->value.y / canvas_size.y) // * 0.5f
        };
        float3 world_scale =
            zox_has(canvas, Scale3) ?
                zox_getv(canvas, Scale3) :
                (zox_has(canvas, Scale1) ?
                    float3_single(zox_getv(canvas, Scale1)) :
                    float3_one);
        scale.x *= world_scale.x;
        scale.y *= world_scale.y;
        set_mesh_vertices_scale3(
            verts,
            get_aligned_mesh2D(alignment->value),
            4,
            scale);
        zox_add(e, MeshDirty);
        if (dbg_log) {
            zox_log("Layout3MeshUpdateSystem Mesh 3D Updated [%s] Size [%fx%f]",
                zox_getn(e),
                scale.x,
                scale.y);
        }
    }
} zox_sys_end(Layout3MeshUpdateSystem);

// NOTE: Initializes Mesh based on instantiated layout size
zox_sys2(Layout3MeshBeginSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    // zox_sys_in(Initialize);
    zox_sys_in(LayoutSize);
    zox_sys_in(MeshAlignment);
    zox_sys_out(MeshVertices);
    // zox_sys_out(MeshDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // zox_sys_i(Initialize, initialize);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(MeshAlignment, alignment);
        //zox_sys_o(MeshDirty, dirty);
        zox_sys_o(MeshVertices, verts);
        /*if (initialize->value != zox_dirty_active) {
            continue;
        }*/
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas3));
        if (!zox_valid(canvas)) {
            zox_loge("Invalid Canvas3 for Layout %s", zox_get_name(e));
            continue;
        }
        // we also set mesh verts here
        float2 canvas_size = int2_to_float2(zox_getv(canvas, LayoutSize));
        float2 scale = (float2) {
            (size->value.x / canvas_size.y), // * 0.5f,
            (size->value.y / canvas_size.y) // * 0.5f
        };
        float3 world_scale = zox_has(canvas, Scale3) ?
            zox_getv(canvas, Scale3) : (zox_has(canvas, Scale1) ?
                float3_single(zox_getv(canvas, Scale1)) :
                float3_one);
        scale.x *= world_scale.x;
        scale.y *= world_scale.y;
        set_mesh_vertices_scale3(verts, get_aligned_mesh2D(alignment->value), 4, scale);
        zox_add(e, MeshDirty);
        // dirty->value = mesh_state_upload;
        if (dbg_log) {
            zox_log("LayoutMeshBeginSystem Mesh 3D Updated [%s] Size [%fx%f] Canvas Scale [%fx%f]", zox_getn(e), scale.x, scale.y, world_scale.x, world_scale.y);
        }
    }
} zox_sys_end(Layout3MeshBeginSystem);

