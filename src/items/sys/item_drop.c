extern entity spawn_pickup_basic(ecs*, float3);
extern entity spawn_pickup_block(ecs*, float3, entity);

// we should drop from actions too!
// TODO: Reparent the item to the new world item object
zox_sys2(ItemDropSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Dead);
    zox_sys_in(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Dead, dead);
        zox_sys_i(Position3D, position);
        // proces if i die
        if (dead->value != zox_dirty_active) {
            continue;
        }
        // drop item
        // zox_log("character dropping [%i] items", items->length)
        uint capacity = zox_children_capacity;
        entity items[capacity];
        uint items_length = zox_get_children_by_id(world, e, items, capacity, zox_id(Item));
        for (int j = 0; j < items_length; j++) {
            entity item = items[j];
            if (zox_valid(item)) {
                zox_get_prefab(item, meta);
                entity pickup;
                if (zox_has(item, BlockLink)) {
                    zox_geter_value(item, BlockLink, entity, block)
                    pickup = spawn_pickup_block(world, position->value, block);
                } else {
                    pickup = spawn_pickup_basic(world, position->value);
                }
                // zox_log("  - [%s] - m [%s]", zox_get_name(item), zox_get_name(meta))
                zox_set(pickup, ItemLink, { meta })
            }
        }
        // destroy voxel sound
        // spawn_sound_generated(world, prefab_sound_generated, instrument_piano, note_frequencies[24 + rand() % 6], 0.4, 1.2f * get_volume_sfx());
    }
} zox_sys_end(ItemDropSystem);
