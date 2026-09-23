entity spawn_item_body(
    ecs *world,
    entity model,
    entity texture,
    const char* name)
{
    entity e = spawn_realm_item2(
        world,
        prefab_item,
        name);
    zox_add(e, ItemVox);
    zox_add(e, BodyPart);
    zox_add(e, BodyItem);
    zox_setv(e, ModelLink, model);
    zox_link(world, e, TextureLink, texture);
    return e;
}

void delayed_texture_generate(ecs* world, entity e) {
    if (zox_valid(e)) {
        zox_setv(e, GenerateTexture, zox_generate_texture_run);
    }
}

entity spawn_texture_from_vox(
    ecs* world,
    entity vox,
    byte2 tsize)
{
    // # # # Spawn Item Texture # # #
    entity texture = spawn_texture(
        world,
        prefab_vox_texture,
        byte2_to_int2(tsize));
    {
        char name2[128];
        sprintf(name2, "texture_%s", zox_getn(vox));
        zox_set_unique_name(texture, name2);
    }
    // zox_set_name_e(texture, "bodys_texture_head");
    zox_setv(texture, VoxBakeSide, direction_front);
    zox_setv(texture, ModelLink, vox);
    delay_event(
        world,
        &delayed_texture_generate,
        texture,
        1.0f);
    return texture;
}

entity2 spawn_realm_body_part(
    ecs* world,
    entity parent,
    byte variants,
    byte mdepth,
    byte3 size,
    entity blueprint,
    const char* name,
    lint seed,
    byte2 tsize,
    byte slot_type)
{
    // entity e2 = zox_prefab_from_parent(world, prefab_model_group);
    entity e = zox_ins(world, prefab_model_group);
    zox_set_unique_name(e, name);
    zox_add(e, BodyModel);
    // zox_make_prefab(e);
    zox_set_parent(world, e, parent);
    entity max_depth_vox = 0;
    ModelLinks models = (ModelLinks) { 0 };
    for (byte j = 0; j < variants; j++) {
        lint vseed = seed + j * 1209;
        color vcolor = (color) { 200, 200, 155, 255 };
        ModelLods mlods2 = (ModelLods) { };
        entity model_lods = spawn_model_lods(
            world,
            e,
            prefab_block_vox, // prefab_invisible_vox,
            vcolor,
            vseed,
            mdepth,
            size,
            name,
            &mlods2);
        zox_set_parent(world, model_lods, e);
        // zox_set_unique_name(mlods, "bodys_mlods_head");
        add_to_ModelLinks(&models, model_lods);
        if (j == 0) {
            max_depth_vox = mlods2.value[mdepth];
        }
        spawn_process_model(
            world,
            blueprint,
            model_lods);
    }
    zox_set_ptr(e, ModelLinks, models);
    // NOTE: Uses first model and highest depth one
    entity model = models.value[0];
    entity texture = spawn_texture_from_vox(
        world,
        max_depth_vox,
        tsize);
    zox_set_parent(world, texture, parent);
    // NOTE: This has GenerateModel on it atm
    // zox_setv(max_depth_vox, GenerateModel, 0);
    // # # # Spawn Item from model and texture # # #
    entity e2 = spawn_item_body(
        world,
        model,
        texture,
        name);
    zox_set_parent(world, e2, parent);
    // zox_make_prefab(e2);
    zox_set(e2, SlotType, { slot_type });
    zox_set(e2, MaxRenderDepth, { mdepth });
    return (entity2) { e2, e };
}

entity2 spawn_realm_body_part2(
    ecs* world,
    entity realm,
    lint seed,
    byte model_depth,
    const char* name,
    byte slot_type,
    entity blueprint,
    float3 blueprint_scale,
    byte dbg_log)
{
    // Model Data
    short model_length = octree_size(model_depth);
    byte3 model_size = byte3_scale3f(byte3_single(model_length), blueprint_scale);
    byte2 texture_size = byte2_single(model_length);
    entity2 e = spawn_realm_body_part(
        world,
        realm,
        1,
        model_depth,
        model_size,
        blueprint,
        name,
        seed,
        texture_size,
        slot_type);
    if (dbg_log) {
        zox_log("Model [%s]:", name);
        zox_log("   Depth [%i] Length [%i]",
            model_depth,
            model_length);
        zox_log("   Size [%ix%ix%i]",
            model_size.x,
            model_size.y,
            model_size.z);
    }
    return e;
}
