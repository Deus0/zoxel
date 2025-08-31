

void set_linked_action(
    ecs *world,
    const entity user,
    const int index,
    const entity element,
    const entity3 frame
) {
    if (!zox_valid(user) || !zox_has(user, ActionLinks)) {
        return;
    }
    zox_mut_begin(user, ActionLinks, datas)
    if (index >= datas->length) {
        return;
    }
    datas->value[index] = element;
    zox_mut_end(user, ActionLinks);

    // link new element to all uis
    if (frame.x) {
        zox_set(frame.x, ItemLink, { element });
    }
    if (frame.y) {
        zox_set(frame.y, ItemLink, { element });
    }
    if (frame.z) {
        // zox_log("Relinking item %s to ui %s", zox_get_name(element), zox_get_name(frame.z));
        zox_set(frame.z, ItemLink, { element });
        // Clear Label
        zox_muter(frame.z, TextData, text_data);
        zox_muter(frame.z, TextDirty, text_dirty);
        dispose_TextData(text_data);
        text_dirty->value = zox_dirty_trigger;
    }
}