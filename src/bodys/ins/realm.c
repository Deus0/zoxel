entity spawn_item_body(ecs *world, entity model, entity texture, const char* name) {
    entity e = spawn_realm_item2(world, prefab_item, name);
    zox_add_tag(e, BodyItem);
    zox_set(e, ModelLink, { model });
    zox_set(e, TextureLink, { texture });
    zox_add_tag(e, BodyPart);
    return e;
}

void delayed_texture_spawn(ecs* world, entity e) {
    if (zox_valid(e)) {
        zox_set(e, GenerateTexture, { zox_dirty_trigger });
    }
}

entity2 spawn_realm_body_part(ecs* world, byte variants, byte mdepth, byte3 size, entity blueprint, const char* name, lint seed, byte2 tsize, byte slot_type) {
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
    entity e = spawn_item_body(world, model, texture, name);
    zox_set(e, SlotType, { slot_type });
    zox_set(e, MaxRenderDepth, { mdepth });
    return (entity2) { e, model_group };
}

entity2 spawn_realm_body_part2(ecs* world, entity realm, lint seed, byte model_depth, const char* name, byte slot_type, entity blueprint, float3 blueprint_scale, byte dbg_log) {
    // Model Data
    byte model_length = powers_of_two_byte[model_depth];
    byte3 model_size = byte3_scale3f(byte3_single(model_length), blueprint_scale);
    byte2 texture_size = byte2_single(model_length);
    entity2 e = spawn_realm_body_part(world, 1, model_depth, model_size, blueprint, name, seed, texture_size, slot_type);
    zox_make_prefab(e.x);
    zox_make_prefab(e.y);
    zox_set_parent(world, e.x, realm);
    zox_set_parent(world, e.y, realm);
    // add_to_ItemLinks(items, spawn.x);
    // add_to_ModelLinks(models, spawn.y);
    if (dbg_log) {
        zox_log("Model [%s]:", name);
        zox_log("   Depth [%i] Length [%i]", model_depth, model_length);
        zox_log("   Size [%ix%ix%i]", model_size.x, model_size.y, model_size.z);
    }
    return e;
}

void zox_dbg_body_part(ecs* world, entity e) {
    if (!zox_valid(e)) {
        return;
    }
    entity vox = get_item_model(world, e);
    if (!zox_valid(vox)) {
        zox_log("Model [%s]: Invalid Vox", zox_get_name(e));
        return;
    }
    int3 model_size = zox_getv(vox, ChunkSize);
    byte model_depth = zox_getv(e, MaxRenderDepth);
    byte model_length = powers_of_two_byte[model_depth];
    byte slot_type = zox_getv(e, SlotType);
    zox_log("Body Part [%s] (%i):", zox_get_name(e), slot_type);
    zox_log("   Depth [%i] Length [%i]", model_depth, model_length);
    zox_log("   Size [%ix%ix%i]", model_size.x, model_size.y, model_size.z);
}
