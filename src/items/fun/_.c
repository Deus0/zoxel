#include "character.c"
#include "label.c"

void set_linked_item(
    ecs *world,
    const entity user,
    const int index,
    const entity e
) {
    if (!zox_valid(user) || !zox_has(user, ItemLinks)) {
        return;
    }
    zox_mut_begin(user, ItemLinks, datas)
    if (index >= datas->length) {
        return;
    }
    datas->value[index] = e;
    zox_mut_end(user, ItemLinks);
}

void link_as_new_item(
    ecs *world,
    const entity element,
    const entity3 frame
) {
    // link new element to all uis
    if (frame.x) {
        zox_set(frame.x, ItemLink, { element });
    }
    if (frame.y) {
        zox_set(frame.y, ItemLink, { element });
    }
    if (frame.z) {
        zox_set(frame.z, ItemLink, { element });
        // Clear Label
        zox_muter(frame.z, TextData, text_data);
        zox_muter(frame.z, TextDirty, text_dirty);
        dispose_TextData(text_data);
        text_dirty->value = zox_dirty_trigger;
    }
}