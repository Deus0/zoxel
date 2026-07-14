// Spawns a item from a block! Can place these and throw them at your enemies
entity spawn_block_item(ecs *world, entity block, byte dbg_log) {
    if (!zox_valid(block) || !zox_has(block, ZoxName)) {
        zox_loge("Block components name? [%i]\n", zox_has(block, ZoxName));
        return 0;
    }
    zox_geter(block, ZoxName, voxel_name);
    entity e = spawn_realm_item2(world, prefab_item_active, voxel_name->value);
    zox_set_unique_name(e, voxel_name->value); // "block_item");
    zox_prefab_addc_user_timings(world, e);
    zox_set(e, WarmupTime, { 0.125f });
    zox_set(e, CooldownTime, { 0.125f });
    entity texture = 0;
    if (zox_has(block, TextureLinks)) {
        zox_geter(block, TextureLinks, textures);
        if (textures->length > 0) {
            texture = textures->value[0];
        }
    }
    if (!texture && zox_has(block, TextureLink)) {
        texture = zox_getv(block, TextureLink);
    }
    if (!zox_valid(texture)) {
        zox_logw("Block Missing Texture [%s]", zox_get_name(block));
        texture = string_hashmap_get(files_hashmap_textures, new_string_data("blank"));
    }
    zox_set(e, TextureLink, { texture });
    zox_add_tag(e, ItemBlock);
    zox_set(e, BlockLink, { block });
    zox_set(e, RaycastRange, { block_place_range });
    zox_set(e, RaycastType, { 1 });
    // actually for grass we want to set itemLink differently
    zox_set(block, ItemLink, { e });
    // zox_set_name(item, zox_get_name(block));
    const char* meta_name = zox_get_name(block);
    zox_set_unique_name(e, meta_name);
    if (dbg_log) {
        zox_log("+ New Block Item [%s] Texture [%s] Meta [%s]", voxel_name->value, zox_get_name(texture), meta_name);
        // convert_zext_to_text(voxel_name->value, voxel_name->length), textures->length)
    }
    // zox_log(" + block item [%s] [%s]\n", zox_get_name(block), zox_get_name(e))
    return e;
}
