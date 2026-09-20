entity spawn_realm_item_equip(ecs *world, entity realm, entity model, entity texture, const char* name, byte slot_type) {
    entity e = spawn_realm_item2(world, prefab_item, name);
    zox_make_prefab(e);
    zox_set_parent(world, e, realm);
    zox_add(e, EquipItem);
    zox_prefab_set(e, SlotType, { slot_type });
    zox_prefab_set(e, ModelLink, { model });
    zox_link(world, e, TextureLink, texture);
    return e;
}
