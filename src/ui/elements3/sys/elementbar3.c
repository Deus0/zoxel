// Sets child frontbar mesh when resized
/*zox_sys2(Elementbar3DSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    zox_sys_in(ElementBar);
    zox_sys_in(ElementBarSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, disabled);
        zox_sys_i(ElementBar, bar);
        zox_sys_i(ElementBarSize, size);
        if (disabled->value) {
            continue;
        }
        entity children[layouts2_children_capacity];
        uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
        if (!children_length) {
            zox_loge("No frontbar detected on UI [%s]", zox_getn(e));
            continue;
        }
        entity front_bar = children[0];
        if (!zox_valid(front_bar)) {
            zox_loge("Invalid frontbar detected on UI [%s]", zox_getn(e));
            continue;
        }
#ifdef zox_safety_checks
        if (!zox_has(front_bar, MeshVertices)) {
            zox_loge("Frontbar has no [MeshVertices] [%s]", zox_getn(front_bar));
            continue;
        }
        if (!zox_has(front_bar, MeshDirty)) {
            zox_loge("Frontbar has no [MeshDirty] [%s]", zox_getn(front_bar));
            continue;
        }
#endif
        // removing this breaks it?!?!
        //if (zox_getv(front_bar, Initialize)) {
        //     continue;
        //}
        float percentage = bar->value;
        float2 scale = size->value;
        float left_offset = - scale.x * (1.0f - percentage) * 0.5f;
        if (!zox_has(front_bar, MeshVertices)) {
            continue;
        }
        zox_mut_begin(front_bar, MeshVertices, verts);
        zox_mut_begin(front_bar, MeshDirty, mesh_dirty);
        mesh_dirty->value = mesh_state_trigger;
        for (byte j = 0; j < 4; j++) {
            verts->value[j] = (float3) { left_offset + square_vertices[j].x * scale.x * percentage, square_vertices[j].y * scale.y, 0 };
        }
        if (dbg_log) {
            zox_log("Elementbar set to PositionX [%f] ScaleX[%f]", left_offset, scale.x);
        }
    }
} zox_sys_end(Elementbar3DSystem);*/
