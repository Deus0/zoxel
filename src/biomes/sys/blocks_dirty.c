zox_sys2(BiomeBlocksDirtySystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(RealmLink);
    zox_sys_in(BlockLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(RealmLink, realm);
        zox_sys_i(BlockLinks, blocks);
        if (generate->value != zox_dirty_end) {
            continue;
        }
        zox_muter(realm->value, BlockLinks, realm_blocks);
        byte index = realm_blocks->length;
        for (int j = 0; j < blocks->length; j++) {
            entity block = blocks->value[j];
            add_to_BlockLinks(realm_blocks, block);
            zox_set(block, BlockIndex, { ++index });
            // zox_log("Block [%s] index [%i]", zox_get_name(block), index);
        }
    }
} zox_sys_end(BiomeBlocksDirtySystem);
