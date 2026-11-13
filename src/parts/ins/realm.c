// a realm item made from a vox model, with its icon texture (body parts and gear)
entity spawn_item_vox(
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
    zox_set_link(world, e, ModelLink, model);
    zox_link(world, e, TextureLink, texture);
    return e;
}

// NOTE: Spawns model group, texture and item, the item from blueprint: a model the blueprint generates
//  - body parts tag it (spawn_realm_body_part), gear too (spawn_realm_item_equip_blueprint)
entity2 spawn_realm_blueprint_item(
    ecs* world,
    entity parent,
    byte variants,
    byte max_depth,
    entity blueprint,
    const char* name,
    lint seed,
    byte2 texture_size,
    byte slot_type)
{
    // entity e2 = zox_prefab_from_parent(world, prefab_model_group);
    entity e = zox_ins(world, prefab_model_group);
    zox_set_unique_name(e, name);
    zox_add(e, ModelGroup);
    zox_set_parent(world, e, parent);
    // the realm owns the blueprint (a node graph), not a model: a cloned model would copy it
    //  (unowned it leaked when a game ended, tst end_game_cleanup)
    zox_set_parent(world, blueprint, parent);
    entity max_depth_vox = 0;
    for (byte j = 0; j < variants; j++) {
        lint vseed = seed + j * 1209;
        color vcolor = (color) { 200, 200, 155, 255 };
        entity model = spawn_model_lods_generated(
            world,
            e,
            name, // "body",
            0,
            vcolor,
            max_depth,
            vseed);
        spawn_process_model(
            world,
            blueprint,
            model);
        if (j == 0) {
            max_depth_vox = model;
        }
    }
    // NOTE: Uses first model and highest depth one
    entity model = max_depth_vox;
    entity texture = spawn_texture_from_vox(
        world,
        max_depth_vox,
        texture_size,
        direction_front);
    zox_set_parent(world, texture, parent);
    // NOTE: This has GenerateModel on it atm
    // zox_setv(max_depth_vox, GenerateModel, 0);
    // # # # Spawn Item from model and texture # # #
    entity item = spawn_item_vox(
        world,
        model,
        texture,
        name);
    zox_set_parent(world, item, parent);
    // zox_make_prefab(e2);
    zox_setv(item, SlotType, slot_type);
    zox_setv(item, MaxRenderDepth, max_depth);
    return (entity2) {
        item,
        e
    };
}

// a body part, a blueprint item the body menu lists (BodyItem)
entity2 spawn_realm_body_part(
    ecs* world,
    entity parent,
    byte variants,
    byte max_depth,
    entity blueprint,
    const char* name,
    lint seed,
    byte2 texture_size,
    byte slot_type)
{
    entity2 e = spawn_realm_blueprint_item(world, parent, variants, max_depth, blueprint, name, seed, texture_size, slot_type);
    zox_add(e.x, BodyPart);
    zox_add(e.x, BodyItem);
    zox_add(e.y, BodyModel);
    return e;
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
        // model_size,
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
