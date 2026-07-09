extern entity spawn_pickup_basic(ecs*, float3);
extern entity spawn_pickup_block(ecs*, float3, entity);

// NOTE: For now we just drop BlockItems!
// we should drop from actions too!
// TODO: Reparent the item to the new world item object
// proces if i die
zox_sys2(ItemDropSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Dead);
    zox_sys_in(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Dead, dead);
        zox_sys_i(Position3D, position);
        if (dead->value != zox_dirty_active) {
            continue;
        }
        uint capacity = zox_children_capacity;
        entity items[capacity];
        uint items_length = zox_get_children_by_id(world, e, items, capacity, zox_id(Item));
        if (dbg_log) {
            zox_log("Character dropping [%i] items", items_length);
        }
        for (int j = 0; j < items_length; j++) {
            entity user_item = items[j];
            if (!zox_valid(user_item) || !zox_has(user_item, ItemBlock)) {
                continue;
            }
            entity meta = zox_get_prefab(world, user_item);
            entity e2;
            if (zox_has(user_item, BlockLink)) {
                zox_geter_value(user_item, BlockLink, entity, block)
                e2 = spawn_pickup_block(world, position->value, block);
            } else {
                e2 = spawn_pickup_basic(world, position->value);
            }
            zox_set(e2, ItemLink, { meta });
            if (zox_has(user_item, Quantity)) {
                byte quantity = zox_getv(user_item, Quantity);
                zox_set(e2, Quantity, { quantity });
            }
            if (dbg_log) {
                zox_log("  - [%s] - m [%s]", zox_get_name(user_item), zox_get_name(meta));
            }
        }
        // destroy voxel sound
        // spawn_sound_generated(world, prefab_sound_generated, instrument_piano, note_frequencies[24 + rand() % 6], 0.4, 1.2f * get_volume_sfx());
    }
} zox_sys_end(ItemDropSystem);
