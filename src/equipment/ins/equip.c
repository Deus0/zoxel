entity spawn_equip_item(
    ecs *world,
    const entity model,
    const entity texture,
    const char* name
) {
    const entity e = spawn_meta_item(
        world,
        prefab_item,
        name
    );
    zox_set(e, ModelLink, { model });
    zox_set(e, TextureLink, { texture });

    return e;
}