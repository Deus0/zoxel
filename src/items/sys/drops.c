extern entity spawn_pickup_basic(ecs*, float3);
extern float item_pickup_scale;
extern entity2 spawn_pickup_block(ecs*, entity, float3, float);
extern entity spawn_item_world(ecs*, entity, entity, float3, float);
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
        if (!items_length) {
            continue;
        }
        if (dbg_log) {
            zox_log("Character dropping [%i] items", items_length);
        }
        entity realm = zox_get_link(world, e, RealmLink);
        for (int j = 0; j < items_length; j++) {
            entity item = items[j];
            if (!zox_valid(item) ) //||
                //!zox_has(item, ItemBlock))
            {
                continue;
            }
            spawn_item_world(
                world,
                realm,
                item,
                position->value,
                item_pickup_scale);
            if (dbg_log) {
                zox_log("  - [%s]", zox_getn(item));
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
            entity block_item = zox_get_link(world, block, ItemLink);
            if (!zox_valid(block_item)) {
                zox_loge("block [%s] has no valid item", zox_get_name(block));
            }
            float3 positionf = byte3_to_float3(update.position);
            float3_scale_p(&positionf, scale->value);
            float3_add_float3_p(&positionf, position->value); // chunk
            float3_add_float3_p(&positionf, float3_single(scale->value * 0.5f));
            // get positionf from local position and depth
            entity pickup = spawn_item_world(
                world,
                realm,
                block_item,
                positionf,
                item_pickup_scale);
            /*entity pickup = spawn_pickup_block(
                world,
                block,
                positionf,
                item_pickup_scale).x;*/
            if (pickup) {
                zox_link(world, pickup, ItemLink, block_item);
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

