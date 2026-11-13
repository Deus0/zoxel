entity spawn_realm_item_equip(
    ecs *world,
    entity realm,
    entity model,
    entity texture,
    const char* name,
    byte slot_type)
{
    entity e = spawn_realm_item2(
        world,
        prefab_item,
        name);
    zox_make_prefab(e);
    zox_set_parent(world, e, realm);
    zox_add(e, EquipItem);
    zox_add(e, ItemVox);
    zox_setv(e, SlotType, slot_type);
    zox_set_link(world, e, ModelLink, model);
    zox_link(world, e, TextureLink, texture);
    return e;
}

// gear made from a blueprint (a shirt), like body parts are (spawn_realm_blueprint_item, doc/body_equips.md)
entity spawn_realm_item_equip_blueprint(
    ecs* world,
    entity realm,
    lint seed,
    byte model_depth,
    const char* name,
    byte slot_type,
    entity blueprint)
{
    const short length = octree_size(model_depth);
    const entity2 e = spawn_realm_blueprint_item(world, realm, 1, model_depth, blueprint, name, seed,
        byte2_single(length), slot_type);
    zox_add(e.x, EquipItem);
    return e.x;
}
