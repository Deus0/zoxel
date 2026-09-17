void link_as_new_item(
    ecs *world,
    entity element,
    entity3 frame)
{
    // link new element to all uis
    if (frame.x) {
        zox_link(world, frame.x, Item, element);
    }
    if (frame.y) {
        zox_link(world, frame.y, Item, element);
    }
    if (frame.z) {
        zox_link(world, frame.z, Item, element);
        // Clear Label
        zox_muter(frame.z, TextData, text_data);
        dispose_TextData(text_data);
        zox_add(frame.z, Dirty);
    }
}
