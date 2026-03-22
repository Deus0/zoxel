entity spawn_item_body(ecs *world, entity model, entity texture, const char* name) {

    entity e = spawn_meta_item(world, prefab_item, name);

    zox_add_tag(e, BodyItem);
    zox_set(e, ModelLink, { model });
    zox_set(e, TextureLink, { texture });

    return e;
}
