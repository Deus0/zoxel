// Spawns a item from a block! Can place these and throw them at your enemies
entity spawn_block_item(
    ecs* world,
    entity parent,
    entity block,
    byte dbg_log)
{
    if (!zox_valid(block) || !zox_has(block, ZoxName)) {
        zox_loge("Block components name? [%i]\n", zox_has(block, ZoxName));
        return 0;
    }
    zox_geter(block, ZoxName, voxel_name);
    entity e = spawn_realm_item2(
        world,
        prefab_item_active,
        voxel_name->value);
    zox_set_unique_name(e, voxel_name->value);
    zox_add(e, ItemBlock);
    zox_prefab_addc_user_timings(world, e);
    zox_setv(e, WarmupTime, 0.125f);
    zox_setv(e, CooldownTime, 0.125f);
    zox_setv(e, RaycastRange, block_place_range);
    zox_setv(e, RaycastType, 1);
    if (zox_has(block, BlockVox)) {
        zox_add(e, ItemVox);
    }
    entity model = zox_has(block, ModelLink) ?
        zox_getv(block, ModelLink) : 0;
    if (model) {
        zox_setv(e, ModelLink, model);
    }
    // Links
    zox_link(world, e, BlockLink, block);
    zox_set_parent(world, e, parent);
    // actually for grass we want to set itemLink differently
    // zox_link(world, block, Item, e);
    // entity first_texture = zox_get_link(world, block, TextureLink);
    entity first_texture = zox_get_child_by_id(
        world,
        block,
        zox_id(Texture));
    /*if (zox_has(block, TextureLinks)) {
        zox_geter(block, TextureLinks, textures);
        if (textures->length > 0) {
            texture = textures->value[0];
        }
    }
    if (!texture && zox_has(block, TextureLink)) {
        texture = zox_getv(block, TextureLink);
    }*/
    if (!zox_valid(first_texture)) {
        zox_logw("Block Missing Texture [%s]",
            zox_getn(block));
        first_texture = string_hashmap_get(
            files_hashmap_textures,
            new_string_data("blank"));
    }
    zox_link(world, e, TextureLink, first_texture);
    // zox_setv(e, TextureLink, first_texture);
    // zox_set_name(item, zox_get_name(block));
    const char* meta_name = zox_getn(block);
    zox_set_unique_name(e, meta_name);
    if (dbg_log) {
        zox_log("+ New Block Item [%s] Texture [%s] Meta [%s] BlockVox [%i]",
            voxel_name->value,
            zox_getn(first_texture),
            meta_name,
            zox_has(block, BlockVox));
    }
    return e;
}
