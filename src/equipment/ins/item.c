entity spawn_equip_item(ecs *world, entity model, entity texture, const char* name) {
    entity e = spawn_realm_item2(world, prefab_item, name);
    zox_add_tag(e, EquipItem);
    zox_set(e, ModelLink, { model });
    zox_set(e, TextureLink, { texture });
    return e;
}
