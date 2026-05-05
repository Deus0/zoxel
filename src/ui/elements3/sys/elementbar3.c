zox_sys2(Elementbar3DSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ElementBar);
    zox_sys_in(ElementBarSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ElementBar, bar);
        zox_sys_i(ElementBarSize, size);

        entity children[layouts2_children_capacity];
        uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);

        if (!children_length) {
            continue;
        }

        entity front_bar = children[0];
        if (!zox_valid(front_bar)) {
            continue;
        }

        zox_geter(front_bar, InitializeElement, initializeElement);
        if (initializeElement->value) {
            continue; // removing this breaks it?!?!
        }
        if (!can_render_ui(world, e)) {
            continue; // disabled for now causes issues
        }

        float percentage = bar->value;
        float2 scale = size->value;
        float left_offset = - scale.x * (1.0f - percentage) * 0.5f;

        if (!zox_has(front_bar, MeshVertices)) {
            continue;
        }
        zox_muter(front_bar, MeshDirty, mesh_dirty);
        if (mesh_dirty->value) {
            continue;
        }
        mesh_dirty->value = mesh_state_trigger;

        zox_muter(front_bar, MeshVertices, verts);
        for (byte j = 0; j < 4; j++) {
            verts->value[j] = (float3) { left_offset + square_vertices[j].x * scale.x * percentage, square_vertices[j].y * scale.y, 0 };
        }
    }
} zox_sys_end(Elementbar3DSystem);