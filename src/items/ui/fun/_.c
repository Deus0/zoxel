// Checks if can place icon in skill frame
// NOTE: Returns 1 if can place, 0 if cannot
byte can_place_icon_in_item_frame(ecs* world, entity frame, entity data) {
    if (!zox_valid(frame)) {
        return 0;
    } else if (!zox_has(frame, ItemFrame)) {
        return 1;
    } else if (!zox_valid(data)) {
        return 1;
    } else if (zox_valid(data)) {
        return zox_has(data, Item);
    } else {
        return 1;
    }
}
