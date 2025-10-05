entity spawn_block_item(
    ecs *world,
    const entity block
) {

    if (!zox_valid(block) || !zox_has(block, ZoxName)) {
        zox_log_error("Block components name? [%i]\n", zox_has(block, ZoxName))
        return 0;
    }

    // get block data
    zox_geter(block, ZoxName, voxel_name);
    // char* voxel_name_char = convert_zext_to_text(voxel_name->value, voxel_name->length);

    // spawn item
    const entity e = spawn_meta_item(
        world,
        prefab_item,
        voxel_name->value
    );
    // free(voxel_name_char);
    zox_name("block_item");
    entity t = 0;
    zox_prefab_addc_user_timings(world, e);
    zox_set(e, WarmupTime, { 0.5f });
    zox_set(e, CooldownTime, { 0.5f });

    if (zox_has(block, TextureLinks)) {
        zox_geter(block, TextureLinks, textures);
        if (textures->length > 0) {
            t = textures->value[0];
        }
    }

    if (!t && zox_has(block, TextureLink)) {
        t = zox_get_value(block, TextureLink);
    }

    if (!t) {
        // zox_log("! warning: [todo] implement vox item textures [%s]", zox_get_name(block))
        t = string_hashmap_get(files_hashmap_textures, new_string_data("blank"));
        // zox_log(" ! block [%s] had no textures [%i]\n",  convert_zext_to_text(voxel_name->value, voxel_name->length), textures->length)
    }

    zox_set(e, TextureLink, { t });
    zox_add_tag(e, ItemBlock);
    zox_set(e, BlockLink, { block });
    // actually for grass we want to set itemLink differently
    zox_set(block, ItemLink, { e });
    // zox_set_name(item, zox_get_name(block));
    zox_name(zox_get_name(block));
    // zox_log(" + block item [%s] [%s]\n", zox_get_name(block), zox_get_name(e))
    // zox_log(" + spawning item for block [%s] textures [%i]\n", convert_zext_to_text(voxel_name->value, voxel_name->length), textures->length)
    return e;
}