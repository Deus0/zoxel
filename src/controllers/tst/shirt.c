// tst: shirt, the player wears the realm's shirt over its chest (doc/body_equips.md)
//  - its chest has a shirt slot (ShirtSlot, body_anchor_over) holding the realm's Shirt, made from a blueprint
//  - the shirt's model has voxels, it covers the chest: wider on x and z, from the chest's bottom
//  - the combined body has the shirt's colours (it was merged over the chest)
//  - no bone of its own (it moves with the chest), the skeleton has one bone a part slot
//  - run: bash bsh/tst.sh shirt
byte zox_tst_player_spawn_ready(ecs* world);   // in _.c, after this is included

static inline entity zox_tst_shirt_character(ecs* world) {
    return zox_valid(dbg_player) ? zox_get_link(world, dbg_player, CharacterLink) : 0;
}

byte zox_tst_shirt_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_waiting = "the player's body combined with its shirt";
    const entity character = zox_tst_shirt_character(world);
    const entity chest_slot = zox_valid(character) ? zox_get_child_by_id(world, character, zox_id(Body)) : 0;
    const entity slot = zox_valid(chest_slot) ? get_equip_slot_in_children(world, chest_slot, zox_id(ShirtSlot)) : 0;
    return zox_valid(slot) && zox_valid(zox_get_link(world, slot, DataLink)) &&
        zox_has(character, BodyDirty) && zox_getv(character, BodyDirty) == zox_generate_body_end &&
        zox_getv(slot, PartSize).x;
}

// slots holding a part, under (and with) a slot, over anchored ones (clothes) apart
static inline int zox_tst_shirt_part_slots(ecs* world, entity slot, int* over) {
    int count = 0;
    if (zox_valid(zox_get_link(world, slot, DataLink))) {
        if (zox_getv(slot, SlotAnchor) == body_anchor_over) {
            (*over)++;
        } else {
            count++;
        }
    }
    entity slots[zox_children_capacity];
    const uint length = zox_get_children_by_id(world, slot, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < length; k++) {
        count += zox_tst_shirt_part_slots(world, slots[k], over);
    }
    return count;
}

void zox_tst_shirt_check(ecs* world) {
    const entity character = zox_tst_shirt_character(world);
    const entity chest_slot = zox_get_child_by_id(world, character, zox_id(Body));
    const entity slot = get_equip_slot_in_children(world, chest_slot, zox_id(ShirtSlot));
    zox_tst_check(zox_valid(slot), "no shirt slot on the player's chest");
    if (!zox_valid(slot)) {
        return;
    }
    zox_tst_check(zox_getv(slot, SlotAnchor) == body_anchor_over, "the shirt slot isn't worn over the chest");
    const entity shirt = zox_get_link(world, slot, DataLink);
    zox_tst_check(zox_has(shirt, Shirt) && zox_has(shirt, EquipItem), "[%s] in the shirt slot isn't a shirt", zox_get_name(shirt));
    const entity model = get_item_model(world, shirt);
    zox_tst_check(zox_valid(model), "the shirt has no model");
    if (!zox_valid(model)) {
        return;
    }
    const VoxelNode* voxels = zox_get(model, VoxelNode);
    zox_tst_check(zox_tst_count_voxels(voxels), "the shirt's model has no voxels");
    // over the chest: wider, from its bottom
    const byte3 chest_position = zox_getv(chest_slot, PartPosition), chest_size = zox_getv(chest_slot, PartSize);
    const byte3 position = zox_getv(slot, PartPosition), size = zox_getv(slot, PartSize);
    zox_log("[tst] chest at [%ix%ix%i] size [%ix%ix%i], shirt at [%ix%ix%i] size [%ix%ix%i]",
        chest_position.x, chest_position.y, chest_position.z, chest_size.x, chest_size.y, chest_size.z,
        position.x, position.y, position.z, size.x, size.y, size.z);
    zox_tst_check(size.x > chest_size.x && size.z > chest_size.z, "the shirt isn't wider than the chest");
    zox_tst_check(position.x < chest_position.x && position.z < chest_position.z, "the shirt doesn't wrap the chest");
    zox_tst_check(position.y == chest_position.y, "the shirt doesn't start at the chest's bottom");
    // merged into the body
    const entity body = zox_get_link(world, character, VoxLink);
    const ColorRGBs* shirt_colors = zox_get(model, ColorRGBs);
    const ColorRGBs* body_colors = zox_get(body, ColorRGBs);
    int found = 0;
    for (int i = 0; i < shirt_colors->length; i++) {
        for (int j = 0; j < body_colors->length; j++) {
            found += color_rgb_equals(shirt_colors->value[i], body_colors->value[j]);
        }
    }
    zox_tst_check(found == shirt_colors->length, "[%i] of the shirt's [%i] colours in the body", found, shirt_colors->length);
    // no bone of its own: one bone a part slot, the shirt moves with the chest's
    const entity skeleton = zox_get_link(world, character, SkeletonLink);
    int over = 0;
    const int parts = zox_tst_shirt_part_slots(world, chest_slot, &over);
    if (zox_valid(skeleton) && zox_has(skeleton, BoneLinks)) {
        const int bones = zox_get(skeleton, BoneLinks)->length;
        zox_log("[tst] [%i] bones, [%i] part slots, [%i] clothes", bones, parts, over);
        zox_tst_check(over == 1, "[%i] clothes on the body, the shirt only", over);
        zox_tst_check(bones == parts, "[%i] bones for [%i] part slots, clothes shouldn't have one", bones, parts);
    }
}
