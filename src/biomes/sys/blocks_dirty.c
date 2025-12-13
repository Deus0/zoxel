void BiomeBlocksDirtySystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(RealmLink);
    zox_sys_in(BlockLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(RealmLink, realm);
        zox_sys_i(BlockLinks, blocks);

        if (generate->value != zox_dirty_active) {
            continue;
        }

        zox_muter(realm->value, BlockLinks, realm_blocks);
        byte index = realm_blocks->length;
        for (int j = 0; j < blocks->length; j++) {
            const entity block = blocks->value[j];
            add_to_BlockLinks(realm_blocks, block);
            zox_set(block, BlockIndex, { ++index });
            zox_log("Block [%s] index [%i]", zox_get_name(block), index);
        }
    }
} zoxd_system2(BiomeBlocksDirtySystem);