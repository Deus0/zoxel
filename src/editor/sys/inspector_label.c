// Keeps the shown inspector rows' component values up to date, every frame
zox_sys2(InspectorLabelSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    zox_sys_in(ComponentType);
    zox_sys_in(EntityTarget);
    zox_sys_in(ComponentTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, render_disabled);
        zox_sys_i(ComponentType, type);
        zox_sys_i(EntityTarget, target);
        zox_sys_i(ComponentTarget, component);
        if (render_disabled->value) {
            continue;
        }
        // the name, parent and children rows don't change
        if (type->value == zox_type_none ||
            type->value == zox_type_name ||
            type->value == zox_type_parent ||
            type->value == zox_type_children)
        {
            continue;
        }
        const entity text = zox_get_child_by_id(world, e, zox_id(TextData));
        if (!zox_valid(text)) {
            continue;
        }
        char* label = fetch_component_label(world, target->value, component->value);
        zox_muter(text, TextData, text_data);
        if (set_text_component(text_data, label)) {
            zox_add(text, Dirty);
        }
        free(label);
    }
} zox_sys_end(InspectorLabelSystem);
