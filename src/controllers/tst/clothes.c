// tst: clothes, the player wears the realm's cubey gear: a shirt over the chest, shorts over the hips, a shoe over each foot
//  - each slot (ShirtSlot, ShortsSlot, ShoeSlot) is worn over its part (body_anchor_over) and holds the right item
//  - each piece covers its part: as wide on x and z or wider, its voxels from 0 (else it sits off it), its model has voxels, its colours are merged into the body
//  - none have a bone of their own (they move with what they cover)
//  - logs the body from the front and the side, and a slice through each piece from above: S shirt, P shorts, B shoes, o skin
//  - run: bash bsh/tst.sh clothes
byte zox_tst_player_spawn_ready(ecs* world);   // in _.c, after this is included

// slots holding a part, under (and with) a slot, over anchored ones (clothes) apart
static inline int zox_tst_clothes_part_slots(ecs* world, entity slot, int* over) {
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
        count += zox_tst_clothes_part_slots(world, slots[k], over);
    }
    return count;
}

#define zox_tst_clothes_max 8
typedef struct {
    entity slot;
    entity item;
    entity model;
} zox_tst_cloth;

// every clothes slot (anchor over) under a slot, depth first
static inline void zox_tst_clothes_find(ecs* world, entity slot, zox_tst_cloth* clothes, int* count) {
    if (zox_getv(slot, SlotAnchor) == body_anchor_over && *count < zox_tst_clothes_max) {
        const entity item = zox_get_link(world, slot, DataLink);
        clothes[(*count)++] = (zox_tst_cloth) { slot, item, zox_valid(item) ? get_item_model(world, item) : 0 };
    }
    entity slots[zox_children_capacity];
    const uint length = zox_get_children_by_id(world, slot, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < length; k++) {
        zox_tst_clothes_find(world, slots[k], clothes, count);
    }
}

byte zox_tst_clothes_ready(ecs* world) {
    if (!zox_tst_player_spawn_ready(world)) {
        return 0;
    }
    zox_tst_waiting = "the player's body combined with its clothes";
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity chest_slot = zox_valid(character) ? zox_get_child_by_id(world, character, zox_id(Body)) : 0;
    if (!zox_valid(chest_slot) || !zox_has(character, BodyDirty) || zox_getv(character, BodyDirty) != zox_generate_body_end) {
        return 0;
    }
    zox_tst_cloth clothes[zox_tst_clothes_max];
    int count = 0;
    zox_tst_clothes_find(world, chest_slot, clothes, &count);
    for (int i = 0; i < count; i++) {
        if (!zox_valid(clothes[i].item) || !zox_getv(clothes[i].slot, PartSize).x) {
            return 0;
        }
    }
    return count > 0;
}

// the letter of the body voxel's colour: the first piece of clothes with it, else skin
static inline char zox_tst_clothes_letter(ecs* world, color_rgb c, const zox_tst_cloth* clothes, int count) {
    for (int i = 0; i < count; i++) {
        const ColorRGBs* colors = zox_get(clothes[i].model, ColorRGBs);
        for (int j = 0; j < colors->length; j++) {
            if (color_rgb_equals(colors->value[j], c)) {
                return zox_has(clothes[i].item, Shirt) ? 'S' : zox_has(clothes[i].item, Shorts) ? 'P' :
                    zox_has(clothes[i].item, Shoes) ? 'B' : '?';
            }
        }
    }
    return 'o';
}

// every part's place and size in the body, to fit clothes to them
static inline void zox_tst_clothes_log_parts(ecs* world, entity slot) {
    const entity part = zox_get_link(world, slot, DataLink);
    if (zox_valid(part) && zox_getv(slot, SlotAnchor) != body_anchor_over) {
        const byte3 position = zox_getv(slot, PartPosition), size = zox_getv(slot, PartSize);
        zox_log("[tst]   part [%s] at [%ix%ix%i] size [%ix%ix%i]", zox_get_name(part),
            position.x, position.y, position.z, size.x, size.y, size.z);
    }
    entity slots[zox_children_capacity];
    const uint length = zox_get_children_by_id(world, slot, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < length; k++) {
        zox_tst_clothes_log_parts(world, slots[k]);
    }
}

// the body seen from the front (+z, nearest voxel) and the side (-x), top row first
void zox_tst_clothes_log_body(ecs* world, entity character, const zox_tst_cloth* clothes, int count) {
    const entity body = zox_get_link(world, character, VoxLink);
    const byte3 size = zox_getv(character, BodySize);
    const byte depth = zox_getv(body, NodeDepth);
    const VoxelNode* voxels = zox_get(body, VoxelNode);
    const ColorRGBs* colors = zox_get(body, ColorRGBs);
    zox_log("[tst] body [%ix%ix%i], front | side", size.x, size.y, size.z);
    char line[600];
    for (int y = size.y - 1; y >= 0; y--) {
        int n = 0;
        for (int x = 0; x < size.x; x++) {
            char letter = ' ';
            for (int z = size.z - 1; z >= 0; z--) {
                const byte v = getv_VoxelNode(voxels, depth, (byte3) { x, y, z });
                if (v) {
                    letter = zox_tst_clothes_letter(world, colors->value[v - 1], clothes, count);
                    break;
                }
            }
            line[n++] = letter;
        }
        line[n++] = ' ';
        line[n++] = '|';
        line[n++] = ' ';
        for (int z = 0; z < size.z; z++) {
            char letter = ' ';
            for (int x = 0; x < size.x; x++) {
                const byte v = getv_VoxelNode(voxels, depth, (byte3) { x, y, z });
                if (v) {
                    letter = zox_tst_clothes_letter(world, colors->value[v - 1], clothes, count);
                    break;
                }
            }
            line[n++] = letter;
        }
        line[n] = 0;
        zox_log("[tst] %3i %s", y, line);
    }
}

// a slice of the body at a height, seen from above (front, +z, at the bottom)
void zox_tst_clothes_log_slice(ecs* world, entity character, int y, const zox_tst_cloth* clothes, int count) {
    const entity body = zox_get_link(world, character, VoxLink);
    const byte3 size = zox_getv(character, BodySize);
    const byte depth = zox_getv(body, NodeDepth);
    const VoxelNode* voxels = zox_get(body, VoxelNode);
    const ColorRGBs* colors = zox_get(body, ColorRGBs);
    zox_log("[tst] slice at y [%i], from above", y);
    char line[300];
    for (int z = 0; z < size.z; z++) {
        int n = 0;
        for (int x = 0; x < size.x; x++) {
            const byte v = getv_VoxelNode(voxels, depth, (byte3) { x, y, z });
            line[n++] = v ? zox_tst_clothes_letter(world, colors->value[v - 1], clothes, count) : ' ';
        }
        line[n] = 0;
        zox_log("[tst] %3i %s", z, line);
    }
}

// whether a model has a voxel on its x = 0 (axis 0) or z = 0 (axis 2) side: a body copies a part's voxels from 0,
//  centred by its size, so a gap there shifts it off its part (build_body_parts, combine_vox_system)
static inline byte zox_tst_clothes_starts_at_0(ecs* world, entity model, byte axis) {
    const byte depth = zox_getv(model, NodeDepth);
    const int3 size = zox_getv(model, ChunkSize);
    const VoxelNode* voxels = zox_get(model, VoxelNode);
    const int across = axis == 0 ? size.z : size.x;
    for (int a = 0; a < across; a++) {
        for (int y = 0; y < size.y; y++) {
            const byte3 position = axis == 0 ? (byte3) { 0, y, a } : (byte3) { a, y, 0 };
            if (getv_VoxelNode(voxels, depth, position)) {
                return 1;
            }
        }
    }
    return 0;
}

void zox_tst_clothes_check(ecs* world) {
    const entity character = zox_get_link(world, dbg_player, CharacterLink);
    const entity chest_slot = zox_get_child_by_id(world, character, zox_id(Body));
    zox_tst_cloth clothes[zox_tst_clothes_max];
    int count = 0;
    zox_tst_clothes_find(world, chest_slot, clothes, &count);
    int shirts = 0, shorts = 0, shoes = 0;
    const entity body = zox_get_link(world, character, VoxLink);
    const ColorRGBs* body_colors = zox_get(body, ColorRGBs);
    for (int i = 0; i < count; i++) {
        const zox_tst_cloth cloth = clothes[i];
        const entity parent_slot = zox_get_parent(world, cloth.slot);
        const entity parent_part = zox_get_link(world, parent_slot, DataLink);
        const byte is_shirt = zox_has(cloth.slot, ShirtSlot) && zox_has(cloth.item, Shirt);
        const byte is_shorts = zox_has(cloth.slot, ShortsSlot) && zox_has(cloth.item, Shorts) && zox_has(parent_part, Hips);
        const byte is_shoe = zox_has(cloth.slot, ShoeSlot) && zox_has(cloth.item, Shoes) &&
            (zox_getv(parent_part, SlotType) == zox_slot_lfoot || zox_getv(parent_part, SlotType) == zox_slot_rfoot);
        zox_tst_check(is_shirt || is_shorts || is_shoe, "[%s] in clothes slot [%i] isn't over its part", zox_get_name(cloth.item), i);
        shirts += is_shirt;
        shorts += is_shorts;
        shoes += is_shoe;
        zox_tst_check(zox_has(cloth.item, EquipItem), "[%s] isn't gear", zox_get_name(cloth.item));
        zox_tst_check(zox_valid(cloth.model) && zox_tst_count_voxels(zox_get(cloth.model, VoxelNode)), "[%s] has no voxels", zox_get_name(cloth.item));
        if (!zox_valid(cloth.model)) {
            continue;
        }
        // covers its part: wider on x and z, around it
        const byte3 parent_position = zox_getv(parent_slot, PartPosition), parent_size = zox_getv(parent_slot, PartSize);
        const byte3 position = zox_getv(cloth.slot, PartPosition), size = zox_getv(cloth.slot, PartSize);
        zox_log("[tst] [%s] at [%ix%ix%i] size [%ix%ix%i], over [%s] at [%ix%ix%i] size [%ix%ix%i]", zox_get_name(cloth.item),
            position.x, position.y, position.z, size.x, size.y, size.z, zox_get_name(parent_part),
            parent_position.x, parent_position.y, parent_position.z, parent_size.x, parent_size.y, parent_size.z);
        zox_tst_check(size.x >= parent_size.x && size.z >= parent_size.z, "[%s] is thinner than its part", zox_get_name(cloth.item));
        zox_tst_check(position.x <= parent_position.x && position.z <= parent_position.z, "[%s] doesn't wrap its part", zox_get_name(cloth.item));
        zox_tst_check(zox_tst_clothes_starts_at_0(world, cloth.model, 0) && zox_tst_clothes_starts_at_0(world, cloth.model, 2),
            "[%s]'s voxels don't start at 0, it sits off its part", zox_get_name(cloth.item));
        // merged into the body
        const ColorRGBs* colors = zox_get(cloth.model, ColorRGBs);
        int found = 0;
        for (int j = 0; j < colors->length; j++) {
            for (int k = 0; k < body_colors->length; k++) {
                found += color_rgb_equals(colors->value[j], body_colors->value[k]);
            }
        }
        zox_tst_check(found == colors->length, "[%i] of [%s]'s [%i] colours in the body", found, zox_get_name(cloth.item), colors->length);
    }
    zox_tst_check(shirts == 1 && shorts == 1 && shoes == 2, "wearing [%i] shirts [%i] shorts [%i] shoes, not 1 1 2", shirts, shorts, shoes);
    // no bones of their own: one bone a part slot
    int over = 0;
    const int parts = zox_tst_clothes_part_slots(world, chest_slot, &over);
    const entity skeleton = zox_get_link(world, character, SkeletonLink);
    if (zox_valid(skeleton) && zox_has(skeleton, BoneLinks)) {
        const int bones = zox_get(skeleton, BoneLinks)->length;
        zox_tst_check(bones == parts, "[%i] bones for [%i] part slots, clothes shouldn't have one", bones, parts);
    }
    zox_tst_clothes_log_parts(world, chest_slot);
    zox_tst_clothes_log_body(world, character, clothes, count);
    for (int i = 0; i < count; i++) {
        const byte3 position = zox_getv(clothes[i].slot, PartPosition), size = zox_getv(clothes[i].slot, PartSize);
        if (i == 0 || !zox_has(clothes[i].item, Shoes)) {
            zox_tst_clothes_log_slice(world, character, position.y + size.y / 2, clothes, count);
        }
    }
}
