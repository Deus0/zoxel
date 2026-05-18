// Checks if can place icon in skill frame
byte can_place_icon_in_item_frame(ecs* world, entity frame, entity data) {
    if (!zox_valid(frame)) {
        return 0;
    } else if (!zox_has(frame, ItemFrame)) {
        return 1;
    } else {
        return zox_has(data, Item);
    }
}
