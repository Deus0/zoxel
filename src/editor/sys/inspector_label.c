// Updates inspector component labels per frame
zox_sys2(InspectorLabelSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(RenderDisabled);
    zox_sys_in(ComponentType);
    zox_sys_in(EntityTarget);
    zox_sys_in(ComponentTarget);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(RenderDisabled, rdisabled);
        zox_sys_i(ComponentType, type);
        zox_sys_i(EntityTarget, target);
        zox_sys_i(ComponentTarget, id);
        if (rdisabled->value) {
            continue;
        }
        if (type->value == zox_type_none || type->value == zox_type_name || type->value == zox_type_parent || type->value == zox_type_children) {
            continue;
        }
        entity children[layouts2_children_capacity];
        uint children_length = zox_get_children(world, e, children, layouts2_children_capacity);
        if (!children_length) {
            continue;
        }
        entity text = children[0];
        // if invalid or dirty, skip
        if (!zox_valid(text) || !zox_has(text, TextData) || !zox_has(text, TextDirty) || zox_getv(text, TextDirty)) {
            continue;
        }
        zox_mut_begin(text, TextData, text_data);
        // zox_geter_id(target->value, id->value, float2, value);
        char* ntext = fetch_compoent_label(world, target->value, id->value);
        if (set_text_component(text_data, ntext)) {
            zox_muter(text, TextDirty, dirty);
            dirty->value = zox_dirty_trigger;
            zox_logv("> Component [%s] Label Updated [%s]",
                id->value ?
                zox_get_name(id->value) :
                "None",
                ntext);
        } else if (ntext) {
            // zox_log("> Component [%s] Label Same [%s]", id->value ? zox_get_name(id->value) : "None", ntext);
            free(ntext);
        }
    }
} zox_sys_end(InspectorLabelSystem);
