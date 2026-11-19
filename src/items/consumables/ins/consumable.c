// Spawns a consumable item: food, a model eaten to restore a stat
entity spawn_item_consumable(
    ecs* world,
    entity parent,
    const char* name,
    entity model,
    byte depth,
    entity texture_vox,
    byte face,
    entity consume_stat,
    float consume_value,
    byte dbg_log)
{
    if (!zox_valid(model)) {
        zox_loge("[spawn_item_consumable] Invalid Model");
        return 0;
    }
    short length = octree_size(depth);
    byte2 texture_size = byte2_single(length);
    entity e = spawn_realm_item2(
        world,
        prefab_item_active,
        name);
    zox_set_unique_name(e, name);
    zox_add(e, ItemVox);
    zox_add(e, ItemConsumable);
    zox_setv(e, RaycastRange, block_place_range);
    zox_setv(e, RaycastType, zox_raycast_type_none);  // only consumed, no gizmo
    zox_prefab_addc_user_timings(world, e);
    zox_setv(e, WarmupTime, 0);         // no cast: eaten at its swing's first bite
    zox_setv(e, CooldownTime, 1.0f);
    zox_setv(e, SwingTime, 1.0f);       // to the mouth, bites
    // Consuming restores a stat
    zox_set(e, ConsumeValue, { consume_value });
    if (zox_valid(consume_stat)) {
        zox_link(world, e, ConsumeStat, consume_stat);
    }
    // Links
    zox_set_link(world, e, ModelLink, model);
    zox_set_parent(world, e, parent);
    // actually for grass we want to set itemLink differently
    entity texture = spawn_texture_from_vox(
        world,
        texture_vox,
        texture_size,
        face);
    zox_set_parent(world, texture, parent);
    zox_link(world, e, TextureLink, texture);
    if (dbg_log) {
        zox_log("+ Consumable [%s]", name);
    }
    return e;
}
