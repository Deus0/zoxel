extern entity spawn_pickup_basic(ecs*, float3);
extern float item_pickup_scale;
extern entity2 spawn_pickup_block(ecs*, entity, float3, float);

// TODO: Move this over to Pickups

// NOTE: For now we just drop BlockItems!
// we should drop from actions too!
// TODO: Reparent the item to the new world item object
// proces if i die
void character_item_drop_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Position3D);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Position3D, position);
        uint capacity = zox_children_capacity;
        entity items[capacity];
        uint items_length = zox_get_children_by_id(
            world,
            e,
            items,
            capacity,
            zox_id(Item));
        if (dbg_log) {
            zox_log("Character dropping [%i] items", items_length);
        }
        for (int j = 0; j < items_length; j++) {
            entity user_item = items[j];
            if (!zox_valid(user_item) ||
                !zox_has(user_item, ItemBlock))
            {
                continue;
            }
            entity meta = zox_get_prefab(
                world,
                user_item);
            entity e2;
            entity block = zox_get_link(
                world,
                user_item,
                BlockLink);
            if (block) {
                e2 = spawn_pickup_block(
                    world,
                    block,
                    position->value,
                    item_pickup_scale).x;
            } else {
                e2 = spawn_pickup_basic(
                    world,
                    position->value);
            }
            zox_link(world, e2, Item, meta);
            if (zox_has(user_item, Quantity)) {
                byte quantity = zox_getv(user_item, Quantity);
                zox_setv(e2, Quantity, quantity);
            }
            if (dbg_log) {
                zox_log("  - [%s] - m [%s]",
                    zox_get_name(user_item),
                    zox_get_name(meta));
            }
        }
        // destroy voxel sound
        // spawn_sound_generated(world, prefab_sound_generated, instrument_piano, note_frequencies[24 + rand() % 6], 0.4, 1.2f * get_volume_sfx());
    }
    zox_sys_on_end();
} zoxd_system(character_item_drop_system);

// SpawnS a pickup when removed
// TODO: This has 17ms try optimize it?
void terrain_item_drop_system(iter* it) {
    byte dbg_log = 0;
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(VoxelNodeQueue);
    zox_sys_in(Position3D);
    zox_sys_in(BlockScale);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(VoxelNodeQueue, drops);
        zox_sys_i(Position3D, position);
        zox_sys_i(BlockScale, scale);
        if (!drops->count) {
            continue;
        }
#ifdef zox_safety_checks
        if (!scale->value) {
            zox_loge("Block Scale 0 in chunk [%s]",
                zox_getn(e));
            continue;
        }
#endif
        entity terrain = zox_get_parent(world, e);
#ifdef zox_safety_checks
        if (!zox_valid(terrain)) {
            zox_loge("Terrain Invalid [TerrainItemDropSystem]");
            continue;
        }
#endif
        entity realm = zox_get_parent(world, terrain);
#ifdef zox_safety_checks
        if (!zox_valid(realm)) {
            zox_loge("Realm Invalid [TerrainItemDropSystem]");
            continue;
        }
#endif
        const BlockLinks* blocks = zox_get(realm, BlockLinks);
        for (int j = drops->count - 1; j >= 0; j--) {
            VoxelNodeUpdate update = drops->ptr[j];
            if (update.state != zox_voxel_queue_post) {
                continue;
            }
            byte voxel = update.old_value;
            if (!voxel) {
                // zox_loge("Voxel Dropped is Air at [%ix%ix%i]", update.position.x, update.position.y, update.position.z);
                continue;
            }
            entity block = blocks->value[voxel - 1];
            if (!zox_valid(block)) {
                zox_loge("TerrainDrop: Block Invalid [%i]",
                    voxel - 1);
                continue;
            }
            entity block_item = zox_get_link(world, block, Item);
            if (!zox_valid(block_item)) {
                zox_loge("block [%s] has no valid item", zox_get_name(block));
            }
            float3 positionf = byte3_to_float3(update.position);
            float3_scale_p(&positionf, scale->value);
            float3_add_float3_p(&positionf, position->value); // chunk
            float3_add_float3_p(&positionf, float3_single(scale->value * 0.5f));
            // get positionf from local position and depth
            entity pickup = spawn_pickup_block(
                world,
                block,
                positionf,
                item_pickup_scale).x;
            if (pickup) {
                zox_link(world, pickup, Item, block_item);
            }
            if (dbg_log) {
                zox_log("Spawned block pickup at [%fx%fx%f] scale [%f]",
                    positionf.x,
                    positionf.y,
                    positionf.z,
                    scale->value);
            }
        }
    }
    zox_sys_on_end();
} zoxd_system(terrain_item_drop_system);

