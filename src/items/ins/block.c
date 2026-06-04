// Spawns a item from a block! Can place these and throw them at your enemies
entity spawn_block_item(ecs *world, entity block) {
    if (!zox_valid(block) || !zox_has(block, ZoxName)) {
        zox_log_error("Block components name? [%i]\n", zox_has(block, ZoxName))
        return 0;
    }
    // get block data
    zox_geter(block, ZoxName, voxel_name);
    // spawn item
    entity e = spawn_realm_item2(world, prefab_item_active, voxel_name->value);
    zox_name("block_item");
    zox_prefab_addc_user_timings(world, e);
    zox_set(e, WarmupTime, { 0.5f });
    zox_set(e, CooldownTime, { 0.5f });
    entity texture = 0;
    if (zox_has(block, TextureLinks)) {
        zox_geter(block, TextureLinks, textures);
        if (textures->length > 0) {
            texture = textures->value[0];
        }
    }
    if (!texture && zox_has(block, TextureLink)) {
        texture = zox_get_value(block, TextureLink);
    }
    if (!texture) {
        // zox_log("! warning: [todo] implement vox item textures [%s]", zox_get_name(block))
        texture = string_hashmap_get(files_hashmap_textures, new_string_data("blank"));
        // zox_log(" ! block [%s] had no textures [%i]\n",  convert_zext_to_text(voxel_name->value, voxel_name->length), textures->length);
    }
    zox_set(e, TextureLink, { texture });
    zox_add_tag(e, ItemBlock);
    zox_set(e, BlockLink, { block });
    zox_set(e, RaycastRange, { block_place_range });
    // actually for grass we want to set itemLink differently
    zox_set(block, ItemLink, { e });
    // zox_set_name(item, zox_get_name(block));
    const char* meta_name = zox_get_name(block);
    zox_set_unique_name(e, meta_name);
    // zox_log(" + block item [%s] [%s]\n", zox_get_name(block), zox_get_name(e))
    // zox_log(" + spawning item for block [%s] textures [%i]\n", convert_zext_to_text(voxel_name->value, voxel_name->length), textures->length)
    return e;
}
