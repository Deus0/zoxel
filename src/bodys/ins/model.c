void delayed_texture_spawn(ecs* world, entity e) {
    if (zox_valid(e)) {
        zox_set(e, GenerateTexture, { zox_dirty_trigger });
    }
}

entity2 spawn_body_model_item(ecs* world, byte variants, byte mdepth, byte3 size, entity blueprint, const char* name, lint seed, byte2 tsize, byte slot_type) {
    zox_make_neww(model_group);
    // zox_set_unique_name(model_group, name);
    zox_add_tag(model_group, BodyModel);
    entity texture_model = 0;
    ModelLinks models = (ModelLinks) { 0 };
    for (byte j = 0; j < variants; j++) {
        lint vseed = seed + j * 1209;
        color vcolor = (color) { 200, 200, 155, 255 };
        ModelLods mlods2 = (ModelLods) { };
        entity mlods = spawn_model_lods(world, vcolor, vseed, mdepth, size, name, &mlods2);
        // zox_set_unique_name(mlods, "bodys_mlods_head");
        add_to_ModelLinks(&models, mlods);
        if (j == 0) {
            texture_model = mlods2.value[mdepth];
        }
        spawn_process_model(world, prefab_process_model, blueprint, mlods);
    }
    zox_set_ptr(model_group, ModelLinks, models);
    entity model = models.value[0];
    // TODO: Generate based on model
    // entity texture = string_hashmap_get(files_hashmap_textures, new_string_data("taskbar_body"));
    // # # # Spawn Item Texture # # #
    entity texture = spawn_texture(world, prefab_vox_texture, byte2_to_int2(tsize));
    // zox_set_name_e(texture, "bodys_texture_head");
    zox_set(texture, VoxBakeSide, { direction_front });
    zox_set(texture_model, TextureLink, { texture });
    // Link Model to Texture
    // zox_set_unique_name(texture_model, "bodys_chest_model_high");
    zox_set(texture, VoxLink, { texture_model });
    // TODO: Spawn Texture with Model Graph
    delay_event(world, &delayed_texture_spawn, texture, 1.0f);
    // # # # Spawn Item from model and texture # # #
    entity ritem = spawn_item_body(world, model, texture, name);
    zox_set(ritem, SlotType, { slot_type });
    return (entity2) { ritem, model_group };
}
