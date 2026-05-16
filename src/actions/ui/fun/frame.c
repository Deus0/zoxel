// note on meta: since just spawned user item, we cant use it yet
/*void on_action_set(ecs* world, entity e, byte index, entity action, entity meta) {
    zox_geter(e, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuActions, menu);
    if (!zox_valid(menu)) {
        return;
    }
    entity children[layouts2_children_capacity];
    uint children_length = zox_get_children(world, menu, children, layouts2_children_capacity);
    if (children_length < 2) {
        return;
    }
    entity body = children[1];
    entity body_children[layouts2_children_capacity];
    uint body_children_length = zox_get_children(world, body, body_children, layouts2_children_capacity);
    if (index >= body_children_length) {
        zox_logw("Index [%i] >= UIs [%i]", index, body_children_length);
        return;
    }
    entity frame = body_children[index];
    entity frame_children[layouts2_children_capacity];
    uint frame_children_length = zox_get_children(world, frame, frame_children, layouts2_children_capacity);
    entity icon = frame_children[0];
    entity label = frame_children_length >= 1 ? frame_children[1] : 0;
    // remember: uses meta item for texture source here
    // zox_get_prefab(action, meta);
    set_icon_from_user_data(world, frame, icon, meta);
    zox_set(icon, DataLink, { action });
    // also set other links
    zox_set(frame, ItemLink, { action });
    zox_set(icon, ItemLink, { action });
    if (label) {
        zox_set(label, ItemLink, { action });
        zox_muter(label, TextData, text_data);
        zox_muter(label, TextDirty, text_dirty);
        dispose_TextData(text_data);
        text_dirty->value = zox_dirty_trigger;
    }
}*/
