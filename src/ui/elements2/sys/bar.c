zox_sys2(Elementbar2System) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ElementBar);
    zox_sys_in(ElementBarSize);
    zox_sys_in(LayoutSize);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(LayoutSize, parent_size);
        zox_sys_i(ElementBar, elementBar);
        zox_sys_i(ElementBarSize, elementBarSize);
        entity children[layouts2_children_capacity];
        uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
        if (!children_length) {
            continue;
        }
        entity front_bar = children[0];
        if (!zox_valid(front_bar)) {
            continue;
        }
        if (!zox_has(front_bar, InitializeElement)) {
            zox_log_error("frontbar missing InitializeElement");
            continue;
        }
        if (!zox_has(front_bar, LayoutPosition)) {
            zox_log_error("frontbar missing LayoutPosition");
            continue;
        }
        if (!zox_has(front_bar, LayoutPositionDirty)) {
            zox_log_error("frontbar missing LayoutPositionDirty");
            continue;
        }
        zox_geter(front_bar, InitializeElement, initializeElement);
        if (initializeElement->value) {
            continue; // removing this breaks it?!?!
        }
        if (!zox_has(front_bar, MeshVertices2D)) {
            continue;
        }
        // important
        if (zox_gett_value(front_bar, MeshDirty) != 0) {
            continue;
        }
        // # Important: Check if busy still
        zox_geter_value(front_bar, GenerateTexture, byte, generate);
        zox_geter_value(front_bar, TextureDirty, byte, tdirty);
        if (generate || tdirty) {
            continue;
        }
        zox_muter(front_bar, LayoutPositionDirty, pdirty);
        zox_muter(front_bar, LayoutSizeDirty, sdirty);
        if (pdirty->value || sdirty->value) {
            continue;
        }
        zox_muter(front_bar, LayoutPosition, position);
        zox_muter(front_bar, LayoutSize, size);
        // we should also set LayoutSize here and dirty for it
        float percentage = elementBar->value;
        float2 scale = elementBarSize->value;
        // const float left_offset = - scale.x * (1.0f - percentage) * 0.5f;
        int2 front_size = parent_size->value; // zox_get_value(e, LayoutSize)
        // calculate margin cause im too lazy to put it here as component yet
        int offset_x = (front_size.x - (front_size.x * scale.x)) / 2;
        float percecentage_2 = ((int) (percentage * 100)) / 100.0f; // only update per 100 units
        front_size.x = (int) floor(scale.x * front_size.x * percecentage_2);
        if (size->value.x != front_size.x) {
            size->value = front_size;
            position->value.x = offset_x;
            pdirty->value = zox_dirty_trigger;
            sdirty->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(Elementbar2System);
